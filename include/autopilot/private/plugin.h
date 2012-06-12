#ifndef AP_PRIVATE_PLUGIN_H
#define AP_PRIVATE_PLUGIN_H

#include <autopilot/autopilot.h>

#if unix || __APPLE_CC__
typedef void *plugin_handle;
#else
# error "Your operating system does not support dynamic library loading (to my knowledge)"
#endif

int autopilot_plugin_init(autopilot_context *ap);

int autopilot_plugin_finalize(autopilot_context *ap);

/* OS-specific functions */
plugin_handle _load_plugin_handle(autopilot_context *ap, const char *plugin_name);

int _is_plugin_handle_load_ok(plugin_handle p);

const char *_plugin_handle_load_error(void);

void *_plugin_handle_lookup_symbol(plugin_handle p, const char *function_name);

void _unload_plugin_handle(plugin_handle p);

#endif
