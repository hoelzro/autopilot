#include <autopilot/private.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* XXX set lua_atpanic */

lua_State *
autopilot_get_lua(autopilot_context *ap)
{
    return ap->L;
}

int
autopilot_lua_init(autopilot_context *ap)
{
    ap->L = luaL_newstate();

    if(! ap->L) {
        return 0;
    }

    luaL_openlibs(ap->L);

    return 1;
}

int
autopilot_lua_finalize(autopilot_context *ap)
{
    lua_close(ap->L);
    return 1;
}

const char *
autopilot_lua_tostring(lua_State *L, int index)
{
    const char *string;

    lua_pushvalue(L, index);
    lua_getglobal(L, "tostring");
    lua_insert(L, -2);
    lua_call(L, 1, 1);
    string = lua_tostring(L, -1);
    lua_pop(L, 1);

    return string;
}

int
autopilot_lua_checkcallable(lua_State *L, int index)
{
    if(lua_type(L, index) == LUA_TFUNCTION) {
        return 1;
    }

    if(luaL_getmetafield(L, index, "__call")) {
        lua_pop(L, 1);
        return 1;
    }

    return luaL_error(L, "expected callable for arg %d (got %s)", index,
        autopilot_lua_tostring(L, index));
}

int
autopilot_lua_absindex(lua_State *L, int index)
{
    if(index < 0) {
        /* this is kinda hack-ish, but we have to do it this way! */
        if(index < LUA_REGISTRYINDEX) {
            return index;
        } else {
            return lua_gettop(L) + index + 1;
        }
    } else {
        return index;
    }
}

int
autopilot_lua_push(lua_State *L, int table, int element)
{
    int length;

    table = autopilot_lua_absindex(L, table);

    length = lua_objlen(L, table);
    lua_pushvalue(L, element);
    lua_rawseti(L, table, length + 1);
}

int
autopilot_lua_push_proxy(lua_State *L)
{
    /* stack: table */
    lua_newtable(L); /* table proxy-table */
    lua_newtable(L); /* table proxy-table metatable */
    lua_pushvalue(L, -3); /* table proxy-table metatable table */
    lua_setfield(L, -2, "__index"); /* table proxy-table metatable */
    lua_setmetatable(L, -2); /* table proxy-table */;
    lua_remove(L, -2); /* proxy-table */

    return 1;
}
