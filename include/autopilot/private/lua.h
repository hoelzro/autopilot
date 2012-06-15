#ifndef AP_PRIVATE_LUA_H
#define AP_PRIVATE_LUA_H

int autopilot_lua_init(autopilot_context *ap);

int autopilot_lua_finalize(autopilot_context *ap);

const char *autopilot_lua_tostring(lua_State *L, int index);

int autopilot_lua_checkcallable(lua_State *L, int index);

int autopilot_lua_push(lua_State *L, int table, int element);

int autopilot_lua_absindex(lua_State *L, int index);

int autopilot_lua_push_proxy(lua_State *L);

int autopilot_lua_iterate(lua_State *L);

int autopilot_lua_getfenv_stack(lua_State *L, int stack_level);

#endif
