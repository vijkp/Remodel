#include <stdio.h>
#include <stdlib.h>
#include "drivers.h"
#include "network.h"
#include "sched.h"
#include "filesystem.h"


int main() {
	///////* change */
    drivers_init();
    drivers_update();
    filesystem_init();
	filesystem_update();
    network_init();
    network_update();

}
