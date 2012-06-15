#include <autopilot.h>
#include <autopilot/private.h>

#include <errno.h>
#include <lauxlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* XXX don't do this! */
static int config_env_ref;

static int
config_env_os_execute(lua_State *L)
{
    if(lua_type(L, 1) == LUA_TTABLE) {
        pid_t pid;

        pid = fork();

        if(pid == -1) {
            lua_pushnil(L);
            lua_pushstring(L, strerror(errno));
            lua_pushinteger(L, errno);

            return 3;
        } else {
            if(pid) {
                int status;

                waitpid(pid, &status, 0);

                if(status) {
                    lua_pushnil(L);
                    lua_pushliteral(L, "execution failed");
                    lua_pushinteger(L, status);

                    return 3;
                } else {
                    lua_pushboolean(L, 1);
                    return 1;
                }
            } else {
                char **argv;
                int nargs;
                int i;

                nargs = lua_objlen(L, 1);
                argv  = lua_newuserdata(L, sizeof(char *) * (nargs + 1));

                for(i = 1; i <= nargs; i++) {
                    const char *arg;

                    lua_rawgeti(L, 1, i);
                    arg         = luaL_checkstring(L, -1);
                    argv[i - 1] = arg; /* we don't need to make a copy, since
                                        * we're keeping the table on the stack
                                        */
                    lua_pop(L, 1);
                }
                argv[nargs] = NULL;

                execvp(argv[0], argv);
                return 0; /* this should never be reached */
            }
        }
    } else {
        lua_pushvalue(L, lua_upvalueindex(1));
        lua_insert(L, 1);
        lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);

        return lua_gettop(L);
    }
}

/* XXX check for things like network/* */
static int
config_env_on(lua_State *L)
{
    int listener_ref;
    const char *signal_name;

    signal_name = luaL_checkstring(L, 1);
    autopilot_lua_checkcallable(L, 2);

    listener_ref = lua_tointeger(L, lua_upvalueindex(1));
    lua_rawgeti(L, LUA_REGISTRYINDEX, listener_ref);
    lua_getfield(L, -1, signal_name);
    if(lua_type(L, -1) == LUA_TNIL) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, signal_name);
    }

    autopilot_lua_push(L, -1, 2);

    return 0;
}

static int
config_env_load_module(lua_State *L)
{
    const char *plugin_name;

    autopilot_context *ap = (autopilot_context *) lua_touserdata(L,
        lua_upvalueindex(1));

    plugin_name = luaL_checkstring(L, 1);

    autopilot_plugin_get(ap, plugin_name);

    return 0;
}

static int
import_symbol(lua_State *L)
{
    lua_settable(L, lua_upvalueindex(1));
    return 0;
}

static int
config_env_import(lua_State *L)
{
    int status;

    lua_getglobal(L, "require"); /* require */
    lua_pushvalue(L, 1); /* require module-name */
    lua_call(L, 1, 1); /* return value of require(module-name) */

    status = autopilot_lua_getfenv_stack(L, 1); /* retval caller-env */
    if(! status) {
        return luaL_error(L, "Unable to fetch caller's environment");
    }

    lua_pushcclosure(L, import_symbol, 1); /* retval import-symbol */
    autopilot_lua_iterate(L);

    return 0;
}

static void
init_config_environment(autopilot_context *ap)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    lua_newtable(L);
    ap->listener_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_newtable(L);

    lua_pushinteger(L, ap->listener_ref);
    lua_pushcclosure(L, config_env_on, 1);
    lua_setfield(L, -2, "on");

    lua_pushlightuserdata(L, ap);
    lua_pushcclosure(L, config_env_load_module, 1);
    lua_setfield(L, -2, "load_module");

    lua_pushcfunction(L, config_env_import);
    lua_setfield(L, -2, "import");

    lua_getglobal(L, "os");
    autopilot_lua_push_proxy(L);
    lua_getfield(L, -1, "execute");
    lua_pushcclosure(L, config_env_os_execute, 1);
    lua_setfield(L, -2, "execute");
    lua_setfield(L, -2, "os");

    lua_newtable(L);
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);

    config_env_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

static void
get_config_environment(lua_State *L)
{
    lua_newtable(L);
    lua_newtable(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, config_env_ref);
    lua_setfield(L, -2, "__index");

    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "__metatable");

    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_G");
}

static int
autopilot_config_load(autopilot_context *ap, const char *filename)
{
    lua_State *L;
    int status;

    L = autopilot_get_lua(ap);

    status = luaL_loadfile(L, filename);
    if(status) {
        autopilot_log_error(ap, "failed to load config file '%s': %s",
            filename, lua_tostring(L, -1));
        lua_pop(L, 1);
        /* XXX indicate error type somehow */
        return 0;
    }

    get_config_environment(L);
    lua_setfenv(L, -2);
    status = lua_pcall(L, 0, 0, 0);

    if(status) {
        autopilot_log_error(ap, "failed to load config file '%s': %s",
            filename, lua_tostring(L, -1));
        lua_pop(L, 1);
        /* XXX indicate error type somehow */
        return 0;
    }
    return 1;
}

int
autopilot_config_init(autopilot_context *ap)
{
    init_config_environment(ap);

    autopilot_log_info(ap, "loading configuration...");
    autopilot_config_load(ap, "config.lua");
    autopilot_log_info(ap, "successfully loaded configuration!");
    return 1;
}

int
autopilot_config_finalize(autopilot_context *ap)
{
    /* no-op */
    return 1;
}
