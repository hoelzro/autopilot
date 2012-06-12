#include <autopilot.h>
#include <autopilot/private.h>

int
autopilot_signal_init(autopilot_context *ap)
{
    autopilot_log_info(ap, "initializing signals");
    return 1;
}

int
autopilot_signal_finalize(autopilot_context *ap)
{
    return 1;
}

void
autopilot_signal_fire(autopilot_context *ap, const char *name, int nargs_on_stack)
{
    lua_State *L;
    int i;
    int lower_limit;
    int upper_limit;

    L = autopilot_get_lua(ap);

    lower_limit = lua_gettop(L) - nargs_on_stack + 1;
    upper_limit = lua_gettop(L);

    autopilot_log_debug(ap, "firing signal %s with %d args", name, nargs_on_stack);
    for(i = lua_gettop(L); i >= lower_limit; i--) {
        const char *converted = autopilot_lua_tostring(L, i);

        autopilot_log_debug(ap, "  L(%03d): %8s %s", i,
            lua_typename(L, lua_type(L, i)), converted);
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, ap->listener_ref);
    lua_getfield(L, -1, name);

    if(lua_type(L, -1) == LUA_TNIL) {
        lua_pop(L, nargs_on_stack + 2); /* two for the other stuff we pushed */
    } else {
        int j;
        int k;
        int status;

        for(j = 1; j <= lua_objlen(L, -1); j++) {
            lua_rawgeti(L, -1, j);
            for(k = lower_limit; k <= upper_limit; k++) {
                lua_pushvalue(L, k);
            }
            status = lua_pcall(L, nargs_on_stack, 0, 0);
            if(status) {
                autopilot_log_error(ap, "error calling handler: %s",
                    lua_tostring(L, -1));

                lua_pop(L, 1);
            }
        }
        lua_pop(L, nargs_on_stack + 2); /* two for the other stuff we pushed */
    }
}
