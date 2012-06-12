#ifndef AP_PRIVATE_H
#define AP_PRIVATE_H

#ifndef COMPILING_AUTOPILOT
# error "You're not supposed to be here..."
#endif

#include <autopilot.h>
#include <autopilot/private/config.h>
#include <autopilot/private/events.h>
#include <autopilot/private/log.h>
#include <autopilot/private/lua.h>
#include <autopilot/private/os.h>
#include <autopilot/private/plugin.h>
#include <autopilot/private/signal.h>

#include <lua.h>
struct event_base;

struct autopilot_context {
    lua_State *L;
    int plugin_ref;   /* reference into Lua registry */
    int config_ref;   /* reference into Lua registry */
    int listener_ref; /* reference into Lua registry */

    struct event_base *event_base;
};

#endif
