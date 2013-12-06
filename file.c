/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : file.c 
* Description: This file contains all functions 
* related to parsing of remodel and processing of 
* targets and source files.
*****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Local Headers */
#include "maindefs.h"
#include "file.h"
#include "misc.h"

/* External variables */
extern target_t       *target_head;
extern srcfile_t      *srcfile_head;
extern remodel_node_t *remodel_head;

error_t file_process_remodelfile() {
    FILE    *fh;
    error_t ret = 0;

    if ((ret = file_get_remodelfile(&fh)) != SUCCESS) {
        LOG("error: 'remodelfile' doesn't exist.\n");
        goto end;
    }
    if ((ret = file_read_remodelfile(&fh)) != SUCCESS) {
        LOG("error: 'remodelfile' read failed.\n");
        goto end;
    }
end:
    if (fh != NULL) fclose(fh);
    return ret;
}

error_t file_get_remodelfile(FILE **remodel_fh) {
    error_t ret = RM_NOFILEENT;

    /* Search for 'Remodel' file in the current directory */
    *remodel_fh = fopen("Remodelfile", "r");
    if (*remodel_fh != NULL) {
        DEBUG_LOG("file found 'remodelfile'.\n");
        ret = SUCCESS;
        goto found;
    }
    *remodel_fh = fopen("remodelfile", "r");
    if (*remodel_fh != NULL) {
        DEBUG_LOG("file found 'remodelfile'.\n");
        ret = SUCCESS;
        goto found;
    }
    return ret;
found:
    return ret;
}

error_t file_read_remodelfile(FILE **fh) {
    char *line = NULL;
    long fsize;
    size_t len=0;
    ssize_t read;
    error_t ret = 0;

    while ((read = getline(&line, &len, *fh)) != -1) {
        ret = file_parse_line(line);
        if (ret != SUCCESS) {
            break;
        }
    }
    if (ret != SUCCESS) {
        goto end;
    }
end:
    free(line);
    return ret;
}

void file_remove_extra_chars(char *line) {
    int  count, len, i, j;
    char *buffer;

    len = strlen(line);
    if (len <= 0) {
        return;
    }
    buffer = (char *)malloc((len + 1)*sizeof(char));
    memset(buffer, '\0', len+1);
    for (i = 0, j = 0; i < len; i++) {
        if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n')) {
            if (line[i] >= ' ') {
                buffer[j] = line[i];
                j++;
            }
        }
    }
    buffer[j+1] = '\0';
    memset(line, '\0', len);
    len = strlen(buffer);
    strncpy(line, buffer, len);
    FREE(buffer);
}

bool file_is_string_empty(char *line) {
    int total = 0;
    int i, len;

    len = strlen(line);
    if (len == 0) {
        return TRUE; 
    }
    for (i=0; i < len; i++) {
        if ((line[i] == ' ') || (line[i] == '\t') || (line[i] == '\n')) {
            total++;
        }
    }
    if(total == len) {
        return TRUE;
    } 
    return FALSE;
}


error_t file_parse_line(char *line) {
    error_t      ret = SUCCESS;
    target_t     *node;
    dependency_t *dp_node;
    char        target_name[128];
    char        *dp_string = NULL;
    char        *command_string = NULL;
    size_t      len = 0;
    int         sscanf_out = 0;
    bool        target_found = false;
    bool        dp_string_found = false;
    bool        command_found = false;
    char        *position1 = NULL;
    char        *position2 = NULL;

    memset(target_name, '\0', 128);
    len = strlen(line);

    if (file_is_string_empty(line)) {
        goto error; /* Return value is still success */
    }
    dp_string = (char *)malloc(len*sizeof(char));
    command_string = (char *)malloc(len*sizeof(char));
    memset(dp_string, '\0', len);
    memset(command_string, '\0', len);

    position1 = strstr(line, "<-:");
    position2 = strstr(line, ":\"");

    if((position1 == NULL) && (position2 == NULL)) {
        sscanf_out = sscanf(line, "%[^'<']<-%[^':']:%*[^\"]\"%[^\"]\"", 
                target_name, dp_string, command_string);
    } else if((position1 != NULL) && (position2 == NULL)) {
        sscanf_out = sscanf(line, "%[^'<']<-:%*[^\"]\"%[^\"]\"",
                target_name, command_string);
    } else if((position1 == NULL) && (position2 != NULL)) {
        sscanf_out = sscanf(line, "%[^'<']<-%[^':']:\"%[^\"]\"", 
                target_name, dp_string, command_string);
    } else if((position1 != NULL) && (position2 != NULL)) {
        sscanf_out = sscanf(line, "%[^'<']<-:\"%[^\"]\"",
                target_name, command_string);
    }

    file_remove_extra_chars(target_name);
    if (!file_is_string_empty(target_name)) {
        target_found = true;
    }
    file_remove_extra_chars(dp_string);
    if (!file_is_string_empty(dp_string)) {
        dp_string_found = true;
    }
    if (!file_is_string_empty(command_string)) {
        command_found = true;
    }
    if (!(target_found && (dp_string_found || command_found))) {
        len = strlen(line);
        if (line[len-1] == '\n') line[len-1] = ' ';
        LOG("error: parsing failed for the line \"%s\"\n", line); 
        ret = RM_PARSE_FAIL;
        goto error;
    }

    /* Build the list of targets */
    node = new_target_node();
    strcpy(node->name, target_name);
    node->command = command_string;

    /* Add to target list */
    node->next = target_head->next;
    target_head->next = node;

    /* Parse dependencies */
    if (dp_string_found) {
        char *dp_name;
        srcfile_t *src_node;
        rm_file_type_t dp_type;
        dp_name = strtok(dp_string, ",");
        while (dp_name != NULL) {
            node->total_dp += 1;
            dp_node = new_dp_node();
            dp_type = check_dp_type(dp_name);
            /* For source files add them to src file list */
            if ((dp_type == RM_TYPE_SRC) || (dp_type == RM_TYPE_HEADER)) {
                src_node = new_src_node();
                strcpy(src_node->name, dp_name);
                if(add_src_node(src_node) == RM_FAIL) {
                    FREE(src_node);
                }
            }
            dp_node->type = dp_type;
            strcpy(dp_node->name, dp_name);
            //DEBUG_LOG("adding dp %s\n", dp_name);

            /* Update this node to dp_head node of the target */
            if (node->dp_head == NULL) {
                dp_node->next = NULL;
                node->dp_head = dp_node;
            } else {
                dp_node->next = node->dp_head;
                node->dp_head = dp_node;
            }
            dp_name = strtok(NULL, ",");
        }
    }
    goto end;
error:
    FREE(command_string);
end:
    FREE(dp_string);
    return ret;
}

target_t *file_get_target(char *target) {
    target_t *node = NULL;
    bool     target_found = false;

    node = target_head->next;
    while (node != NULL) {
        if (strcmp(node->name, target) == 0) {
            target_found = true;
            goto end;
        }
        node = node->next;
    }
end:
    return node;
}

srcfile_t *file_get_srcfile(char *srcfile) {
    srcfile_t *node = NULL;
    bool     srcfile_found = false;

    node = srcfile_head->next;
    while (node != NULL) {
        if (strcmp(node->name, srcfile) == 0) {
            srcfile_found = true;
            goto end;
        }
        node = node->next;
    }
end:
    return node;
}

error_t file_update_src_md5info(char *src_name, char *md5hash) {
    error_t   ret = SUCCESS;
    srcfile_t *node = NULL;
    bool      src_found = false;

    node = srcfile_head->next;
    while ((node != NULL) && (src_found == false)) {
        if(strcmp(node->name, src_name) == 0) {
            src_found = true;
            if (strcmp(node->md5hash, md5hash) != 0) {
                DEBUG_LOG("source file '%s' changed since the last build.\n", src_name);
                node->md5_changed = true;
            } else {
                node->md5_changed = false;
            }
        }
        node = node->next;
    }
end:
    return ret;
}

int file_mark_all_targets_for_build(remodel_node_t *node) {
    error_t     ret = RM_SUCCESS;
    target_t    *target = NULL;
    srcfile_t   *srcfile = NULL;
    int         i;
    int         changes = 0;

    switch (node->type) {
        case RM_TYPE_UNKNOWN:
        case RM_TYPE_TARGET:
            target = node->target;
            if (node->child_nodes == 0) {
                target->build_state = RM_BUILD_READY;
                DEBUG_LOG("node:%s build_state changed to %d\n", 
                        target->name, target->build_state);
                goto end;
            }
            for (i = 0; i < node->child_nodes; i++) {
                if (node->children[i]) {
                    changes +=
                        file_mark_all_targets_for_build(node->children[i]);
                }
            }
            if (changes == 0) {
                target->build_state = RM_BUILD_DONE;
                DEBUG_LOG("node: %s doesn't need build\n", node->name);
            } else {
                target->changed_dp  = changes;
                target->build_state = RM_BUILD_READY;
                DEBUG_LOG("node: %s needs build.\n", node->name);
                changes = 1;
            }
            break;
        case RM_TYPE_SRC:
        case RM_TYPE_HEADER:
            srcfile = node->srcfile;
            if (srcfile->md5_present == false) {
                DEBUG_LOG("node: new srcfile %s is added\n", node->name);
                changes++;
                goto end;
            }
            if (srcfile->md5_changed == true) {
                DEBUG_LOG("node: srcfile %s changed\n", node->name);
                changes++;
                goto end;
            }
            break;
    }
end:
    return changes;
}

error_t file_create_dependency_graph(target_t *target) {
    error_t         ret = RM_SUCCESS;
    target_t        *target_node = NULL;
    srcfile_t       *src_node = NULL;
    remodel_node_t  *rm_node = NULL;

    if (remodel_head) {
        remodel_head->type = RM_TYPE_TARGET;
        remodel_head->target = target;
        strcpy(remodel_head->name, target->name);
        remodel_head->parent = NULL;
    }
    /* For each target node build the dependency tree */
    ret = file_add_nodes_to_remodel(remodel_head);
    if (ret != SUCCESS) {
        goto end;
    }
end:
    return ret;
}


error_t file_add_nodes_to_remodel(remodel_node_t *rm_node) {
    error_t        ret = SUCCESS;
    int            i = 0;
    remodel_node_t *node = NULL;
    target_t       *tnode = NULL;
    srcfile_t      *snode = NULL;
    dependency_t   *dpnode = NULL;

    /* Check the type of node */
    if ((rm_node->type == RM_TYPE_SRC) || (rm_node->type == RM_TYPE_HEADER)) {
        rm_node->child_nodes = 0;
        DEBUG_LOG("source file '%s' has no dependencies.\n", rm_node->srcfile->name);
        ret = SUCCESS;
        goto end;
    } else if (rm_node->type == RM_TYPE_TARGET) {
        if (rm_node->target->total_dp == 0) {
            DEBUG_LOG("target '%s' has no dependencies\n",
                    rm_node->target->name);
            ret = SUCCESS;
            goto end;
        }
        rm_node->child_nodes = rm_node->target->total_dp;
        rm_node->children =
            (remodel_node_t **)malloc((rm_node->child_nodes)*sizeof(remodel_node_t *));
        tnode = rm_node->target;
        dpnode = tnode->dp_head;
        i = 0;
        while (dpnode != NULL) {
            node = new_remodel_node();
            node->type = dpnode->type;
            node->parent = rm_node;
            strcpy(node->name, dpnode->name);

            switch (dpnode->type) {
                case RM_TYPE_TARGET:
                    node->target = file_get_target(dpnode->name);
                    if (node->target == NULL) {
                        LOG("error: target or dependency '%s' is invalid or "
                                "doesn't exist.\n", dpnode->name);
                        ret = RM_FAIL;
                        goto fail;
                    }
                    break;
                case RM_TYPE_SRC:
                case RM_TYPE_HEADER:
                    node->srcfile = file_get_srcfile(dpnode->name);
                    if (node->srcfile == NULL) {
                        LOG("error: source file '%s' is invalid or doesn't exist.\n",
                                dpnode->name);
                        ret = RM_FAIL;
                        goto fail;
                    }
                    break;
            }
            /* Check for cyclic dependency before adding the node */
            if (file_check_cyclic_dependency(node)) {
                /* Detected a cyclic dependency. Bail out. */
                LOG("error: cyclic dependency detected for target/file '%s'\n", node->name); 
                file_print_detected_dependency(node);
                ret = RM_FAIL;
                FREE(node);
                goto fail;
            }
            rm_node->children[i] = node;
            dpnode = dpnode->next;
            i++;
        }
        for (i = 0; i < rm_node->child_nodes; i++) {
            ret = file_add_nodes_to_remodel(rm_node->children[i]); 
            if (ret != SUCCESS) {
                goto end;
            }
        }
    } else {
        /* UNKNOWN NODE TYPE */
        goto end;
    }
end:
    return ret;

fail:
    FREE(rm_node);
    return ret;
}

bool file_check_cyclic_dependency(remodel_node_t *rmnode) {
    remodel_node_t *rmparent;
    char           *name;

    name = rmnode->name;
    rmparent = rmnode->parent; 
    while (rmparent != NULL) {
        if(strcmp(name, rmparent->name) == 0) {
            /* Cyclic dependency detected. Bail out. */
            return true;       
        }
        rmparent = rmparent->parent;
    }
    return false;
}

void file_print_root_to_node(remodel_node_t *node) {
    if (!(node && node->parent)) {
        printf("%s ", node->name);
        return;
    } else {
        file_print_root_to_node(node->parent);
        printf("-> ");
        printf("%s ", node->name);
    }
}

void file_print_detected_dependency(remodel_node_t *node) {
    remodel_node_t *rmparent;
    char           *name;
    
    LOG("info: detected cyclic dependency: ");
    file_print_root_to_node(node);
    printf("\n");

}

void file_cleanup_nodes_for_unchanged_files(remodel_node_t *node) {
    /* remove unwanted nodes */
    file_remove_unnecessary_nodes(node);

    /*  updatethe children count to each node in the tree */
    file_update_children_count(node);
}

void file_remove_unnecessary_nodes(remodel_node_t *node) {
    target_t        *target = NULL;
    srcfile_t       *srcfile = NULL;
    remodel_node_t  *parent = NULL;
    int             i;

    if(node == NULL) {
        DEBUG_LOG("node: stale node\n");
        return;
    }
    switch (node->type) {
        case RM_TYPE_UNKNOWN:
        case RM_TYPE_TARGET:
            target = node->target;
            if (target->changed_dp == 0) {
                if (node->child_nodes == 0) {
                    return;
                }
                /* remove all the dependency nodes */
                for (i = 0; i < node->child_nodes; i++) {
                    if (node->children[i]) {
                        file_remove_unnecessary_nodes(node->children[i]);
                    }
                }
                /* remove the node itself */
                /* set parents pointer to this target to null */
                if (node->parent) {
                    parent = node->parent;
                    for (i = 0; i < parent->child_nodes; i++) {
                        if (parent->children[i] == node) {
                            parent->children[i] = NULL;
                            FREE(node);
                            return;
                            break;
                        }
                    }
                }
            } else {
                for (i = 0; i < node->child_nodes; i++) {
                    if (node->children[i]) {
                        file_remove_unnecessary_nodes(node->children[i]);
                    }
                }
            }
            return;
            break;
        case RM_TYPE_SRC:
        case RM_TYPE_HEADER:
            srcfile = node->srcfile;
            //if (srcfile->md5_changed == false) {
            //DEBUG_LOG("node: %s removed.\n", node->name);
            if (node->parent) {
                parent = node->parent;
                for (i = 0; i < parent->child_nodes; i++) {
                    if (parent->children[i] == node) {
                        parent->children[i] = NULL;
                        FREE(node);
                        return;
                        break;
                    }
                }
            }
            //}
            break;
    }
}

void file_update_children_count(remodel_node_t *node) {
    remodel_node_t  **new_children = NULL;
    remodel_node_t  **old_children = NULL;
    int             child_prev = 0;
    int             child_after = 0;
    int             i, j;

    if (node == NULL) {
        return;
    }
    child_prev = node->child_nodes;
    if (child_prev == 0) {
        return;
    }
    /* count the number of valid child nodes */
    for (i = 0; i < child_prev; i++) {
        if (node->children[i] != NULL) {
            child_after++;
        }
    }
    if (child_prev != child_after) {
        /* update child_nodes */
        node->child_nodes = child_after;
        new_children = (remodel_node_t **)malloc((node->child_nodes)*
                sizeof(remodel_node_t *));
        /* now update the children pointers*/
        for (i = 0, j = 0; i < child_prev; i++) {
            if (node->children[i] != NULL) {
                new_children[j] = node->children[i];
                j++;
            }
        }
        old_children   = node->children; 
        node->children = new_children;
        FREE(old_children);
    }
    /* now call the children nodes */
    for (j = 0; j < node->child_nodes; j++) {
        if (node->children[j] != NULL) {
            file_update_children_count(node->children[j]);
        }
    }
}

/* Deprecated. Not tested well. */
bool file_print_all_leaf_nodes(remodel_node_t *node) {
    target_t    *target = NULL;
    srcfile_t   *srcfile = NULL;
    int         i;

    if(node == NULL) {
        return true;
    }
    switch (node->type) {
        case RM_TYPE_UNKNOWN:
        case RM_TYPE_TARGET:
            target = node->target;
            if (node->child_nodes == 0) {
                DEBUG_LOG("node:%s leaf node\n", target->name);
                return true;;
            }
            for (i = 0; i < node->child_nodes; i++) {
                if (node->children[i]) {
                    if (file_print_all_leaf_nodes(node->children[i])) {
                    }
                }
            }
            break;
        case RM_TYPE_SRC:
        case RM_TYPE_HEADER:
            srcfile = node->srcfile;
            DEBUG_LOG("node: leaf node %s\n", node->name);
            return true;
            break;
    }
    return false;
}

