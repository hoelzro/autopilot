#include <autopilot/private.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLUGIN_BUNDLE_MT_NAME "PluginBundle"

struct plugin_bundle {
    struct autopilot_plugin *plugin;
    plugin_handle handle;
};

#define GET_PLUGIN_BUNDLE(L, index)\
    ((struct plugin_bundle *) luaL_checkudata(L, index, PLUGIN_BUNDLE_MT_NAME))

static autopilot_plugin *
check_if_already_loaded(autopilot_context *ap, const char *plugin_name, const char **error)
{
    struct plugin_bundle *bundle = NULL;

    lua_State *L = autopilot_get_lua(ap);

    *error = NULL;

    lua_rawgeti(L, LUA_REGISTRYINDEX, ap->plugin_ref);
    lua_getfield(L, -1, plugin_name);

    if(! lua_isnil(L, -1)) {
        if(lua_isstring(L, -1)) {
            *error = lua_tostring(L, -1);
        } else {
            bundle = GET_PLUGIN_BUNDLE(L, -1);
        }
    }

    lua_pop(L, 2);

    if(bundle) {
        return bundle->plugin;
    } else {
        return NULL;
    }
}

static void
record_plugin_load_failure(autopilot_context *ap, const char *plugin_name, const char *error)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    lua_rawgeti(L, LUA_REGISTRYINDEX, ap->plugin_ref);
    lua_pushstring(L, error);
    lua_setfield(L, -2, plugin_name);
    lua_pop(L, 1);
}

static void
record_plugin_load_success(autopilot_context *ap, const char *plugin_name, autopilot_plugin *plugin, plugin_handle handle)
{
    lua_State *L;
    struct plugin_bundle *bundle;

    L = autopilot_get_lua(ap);

    lua_rawgeti(L, LUA_REGISTRYINDEX, ap->plugin_ref);
    bundle = lua_newuserdata(L, sizeof(struct plugin_bundle));
    luaL_getmetatable(L, PLUGIN_BUNDLE_MT_NAME);
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, plugin_name);
    lua_pop(L, 1);

    bundle->plugin = plugin;
    bundle->handle = handle;
}

autopilot_plugin *
autopilot_plugin_get(autopilot_context *ap, const char *plugin_name)
{
    autopilot_plugin *plugin = NULL;
    plugin_handle handle;
    const char *error;
    char *plugin_symbol_name;
    char *p;

    plugin = check_if_already_loaded(ap, plugin_name, &error);

    autopilot_log_info(ap, "loading plugin %s...", plugin_name);

    if(plugin) {
        autopilot_log_info(ap, "using already-loaded plugin %s", plugin_name);
        return plugin;
    }
    if(error) {
        autopilot_log_error(ap, "tried loading %s before: %s", plugin_name, error);
        return NULL;
    }

    handle = _load_plugin_handle(ap, plugin_name);
    if(! _is_plugin_handle_load_ok(handle)) {
        const char *error = _plugin_handle_load_error();

        autopilot_log_warn(ap, "failed to load plugin %s: %s", plugin_name,
            error);
        record_plugin_load_failure(ap, plugin_name, error);
        return NULL;
    }

    plugin_symbol_name = malloc(strlen(plugin_name) + sizeof("autopilot__plugin"));
    sprintf(plugin_symbol_name, "autopilot_%s_plugin", plugin_name);
    p = plugin_symbol_name;
    while((p = strchr(p, '-'))) {
        *p = '_';
    }

    autopilot_log_debug(ap, "looking for symbol %s in %s",
        plugin_symbol_name, plugin_name);

    plugin = (autopilot_plugin *) _plugin_handle_lookup_symbol(handle, plugin_symbol_name);

    if(! plugin) {
        autopilot_log_error(ap, "Unable to find symbol %s in plugin %s",
            plugin_symbol_name, plugin_name);
        free(plugin_symbol_name);
        record_plugin_load_failure(ap, plugin_name, "Unable to find symbol");
        return NULL;
    }
    free(plugin_symbol_name);

    if(plugin->required_version.major != AUTOPILOT_VERSION_MAJOR ||
       plugin->required_version.minor != AUTOPILOT_VERSION_MINOR)
    {
        autopilot_log_error(ap, "This instance of autopilot (version %d.%d) does not meet the requirements for the %s plugin (requires %d.%d)",
            AUTOPILOT_VERSION_MAJOR, AUTOPILOT_VERSION_MINOR, plugin_name,
            plugin->required_version.major, plugin->required_version.minor);

        _unload_plugin_handle(handle);
        record_plugin_load_failure(ap, plugin_name, "Version requirements not met");
        return NULL;
    }

    if(plugin->init) {
        int status;

        status = plugin->init(ap);

        if(! status) {
            _unload_plugin_handle(handle);
            record_plugin_load_failure(ap, plugin_name, "plugin init failed");
            return NULL;
        }
    } else {
        autopilot_log_info(ap, "init function not found");
    }

    autopilot_log_info(ap, "loaded plugin %s, version %d.%d", plugin_name,
        plugin->version.major, plugin->version.minor);

    record_plugin_load_success(ap, plugin_name, plugin, handle);

    return plugin;
}

static int
unload_plugin_bundle(lua_State *L)
{
    struct plugin_bundle *bundle;

    bundle = GET_PLUGIN_BUNDLE(L, 1);

    _unload_plugin_handle(bundle->handle);

    return 0;
}

int
autopilot_plugin_init(autopilot_context *ap)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    lua_newtable(L);
    ap->plugin_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    luaL_newmetatable(L, PLUGIN_BUNDLE_MT_NAME);
    lua_pushcfunction(L, unload_plugin_bundle);
    lua_setfield(L, -2, "__gc");

    lua_pop(L, 1);

    return 1;
}

int
autopilot_plugin_finalize(autopilot_context *ap)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    lua_pushnil(L);
    lua_rawseti(L, LUA_REGISTRYINDEX, ap->plugin_ref);

    lua_gc(L, LUA_GCCOLLECT, 0);

    return 1;
}
