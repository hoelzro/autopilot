#include <autopilot/private.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PLUGIN_PATH "./plugins/"

plugin_handle
_load_plugin_handle(autopilot_context *ap, const char *plugin_name)
{
    /* this has enough space for .dylib as well */
    char *plugin_path = malloc(strlen(plugin_name) + sizeof("./plugins/.dylib"));
    plugin_handle handle;

    sprintf(plugin_path, PLUGIN_PATH "%s.so", plugin_name);

    autopilot_log_debug(ap, "trying to load %s", plugin_path);

    handle = dlopen(plugin_path, RTLD_NOW);

#if __APPLE_CC__
    if(! handle) {
        autopilot_log_info(ap, "loading .so failed; trying .dylib");
        sprintf(plugin_path, PLUGIN_PATH "%s.dylib", plugin_name);

        autopilot_log_debug(ap, "trying to load %s", plugin_path);

        handle = dlopen(plugin_path, RTLD_NOW);
    }
#endif

    free(plugin_path);

    return handle;
}

int
_is_plugin_handle_load_ok(plugin_handle p)
{
    return p ? 1 : 0;
}

const char *
_plugin_handle_load_error(void)
{
    return dlerror();
}

void *
_plugin_handle_lookup_symbol(plugin_handle p, const char *function_name)
{
    return dlsym(p, function_name);
}

void
_unload_plugin_handle(plugin_handle p)
{
    dlclose(p);
}
