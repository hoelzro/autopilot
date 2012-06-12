#ifndef AP_PLUGIN_H
#define AP_PLUGIN_H

#include <autopilot/autopilot.h>

struct autopilot_plugin_internals;

struct autopilot_plugin {
    init_function init;
    finalize_function finalize;
    struct autopilot_version version;
    struct autopilot_version required_version;
};

typedef struct autopilot_plugin autopilot_plugin;

autopilot_plugin *autopilot_plugin_get(autopilot_context *ap,
    const char *plugin_name);

#endif
