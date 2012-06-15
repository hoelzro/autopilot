#include <autopilot/private.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct autopilot_system {
    init_function init;
    finalize_function finalize;
};

struct autopilot_system systems[] = {
    { autopilot_log_init,    autopilot_log_finalize    },
    { autopilot_events_init, autopilot_events_finalize },
    { autopilot_lua_init,    autopilot_lua_finalize    },
    { autopilot_plugin_init, autopilot_plugin_finalize },
    { autopilot_config_init, autopilot_config_finalize },
    { NULL, NULL }
};

static int
setup_systems(autopilot_context *ap)
{
    struct autopilot_system *system = systems;
    int status;

    while(system->init) {
        status = system->init(ap);
        if(! status) {
            break;
        }
        system++;
    }

    if(system->init) { /* we broke the loop early */
        system--; /* we assume that the system that failed to initialize
                     has already cleaned up after itself */
        while(system >= systems) {
            system->finalize(ap);
            system--;
        }
        return 0;
    }
    return 1;
}

static int
teardown_systems(autopilot_context *ap)
{
    struct autopilot_system *system;

    system = systems + sizeof(systems) / sizeof(struct autopilot_system) - 2;

    while(system >= systems) {
        /* we ignore the return value for now */
        system->finalize(ap);
        system--;
    }

    return 1;
}

int
main(int argc, char **argv)
{
    autopilot_context ap;
    int status;
    uid_t me;

    me = getuid();
    if(me == 0) {
        fprintf(stderr, "I can't run as root! Sorry!\n");
        exit(1);
    }

    memset(&ap, 0, sizeof(autopilot_context));

    status = setup_systems(&ap);

    if(! status) {
        return 1;
    }

    autopilot_events_run(&ap);

    teardown_systems(&ap); /* ignore the return value for now */

    return 0;
}
