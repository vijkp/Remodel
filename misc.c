/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : misc.c 
* Description: Contains miscellaneous functions. 
*****************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "maindefs.h"

extern remodel_node_t *remodel_head;

int DEBUG_LOG_(char *filename, int ln, const char *format, ...) {
    va_list arg;
    int result;
    char format_new[200];
    
    sprintf(format_new, "remodel: %s +%d: %s", filename, ln, format);
    format = format_new;
    va_start(arg, format);
    result = vprintf(format, arg);
    va_end(arg);
    return result;
}

int LOG(const char *format, ...) {
    va_list arg;
    int result;
    char format_new[200];
    
    sprintf(format_new, "remodel: %s", format);
    format = format_new;
    va_start(arg, format);
    result = vprintf(format, arg);
    va_end(arg);
    return result;
}

void print_usage() {
    LOG("Usage: remodel [<target_name> | --help | -h]\n");
    printf("         Description: <target_name> is optional. Default value 'DEFAULT'\n");
    printf("                      is used when none specified.\n");
    printf("         Options: '-h' and '--help' will print this message\n");
}

/* Print target list along with dependencies */
void print_target_list() {
    target_t        *temp = target_head->next;
    dependency_t    *dp_temp = NULL;
    while (temp != NULL) {
        DEBUG_LOG("target: %-18s (total_dp:%d) command: %s\n", temp->name,
                temp->total_dp, temp->command);
        dp_temp = temp->dp_head;
        DEBUG_LOG("\tdependencies:\n");
        while(dp_temp != NULL) {
            DEBUG_LOG("\t\t%s\n", dp_temp->name);
            dp_temp = dp_temp->next;
        }
        temp = temp->next;
    }

}

/* Print src file list along with md5 hashes, if present */
void print_srcfile_list() {
    srcfile_t *temp2 = srcfile_head->next;
    DEBUG_LOG("List of source files:\n");
    while (temp2 != NULL) {
        DEBUG_LOG("\tfile: %-18s md5: %s\n", temp2->name,
                temp2->md5hash);
        temp2 = temp2->next;
    }

}

int print_dependency_graph(remodel_node_t *node, int level) {
    int i;
    int total_nodes = 0;
    char parentname[MAX_FILENAME] = "";
    char pad[200] = "";
    if (node->parent) {
        strcpy(parentname, node->parent->name);
    }
    for (i = 0; i < level; i++) {
        strcat(pad, "\t");
    }
    switch (node->type) {
        case RM_TYPE_UNKNOWN:
        case RM_TYPE_TARGET:
            DEBUG_LOG("dependency tree: %s -> (state:%d, "
                    "changed_dp:%d N=%d) %s\n", pad, 
                    node->target->build_state, 
                    node->target->changed_dp, node->child_nodes, node->name);
            total_nodes++;
            if (node->child_nodes == 0) {
                goto end;
            }
            level++;
            for (i = 0; i < node->child_nodes; i++) {
                if (node->children[i]) {
                    total_nodes += print_dependency_graph(node->children[i], level);
                }
            }
            break;
        case RM_TYPE_SRC:
        case RM_TYPE_HEADER:
            DEBUG_LOG("dependency tree: %s -> (md5_changed:%d) %s\n", 
                    pad, node->srcfile->md5_changed,
                    node->name);
            total_nodes++;
            goto end;
            break;
    }
end: 
    return total_nodes;
}
