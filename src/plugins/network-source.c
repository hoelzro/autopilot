#include <autopilot.h>
#include <autopilot/plugin/network-source.h>
#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* XXX only supports IPv4 at the moment! */

#define IP_ADDRESS_METATABLE "IPv4Address"
#define GET_IP_ADDRESS(L, index)\
    ((struct in_addr *) luaL_checkudata(L, index, IP_ADDRESS_METATABLE))

static int
lua_ip_address_tostring(lua_State *L)
{
    struct in_addr *address = GET_IP_ADDRESS(L, 1);
    char addr_as_string[INET_ADDRSTRLEN];

    if(! inet_ntop(AF_INET, address, addr_as_string, INET_ADDRSTRLEN)) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        lua_pushinteger(L, errno);
        return 3;
    }
    lua_pushstring(L, addr_as_string);
    return 1;
}

static int
lua_ip_address_match(lua_State *L)
{
    char *cidr_string;
    char *slash_location;
    int mask_len;

    struct in_addr *address;
    struct in_addr subnet;
    struct in_addr mask;

    address     = GET_IP_ADDRESS(L, 1);
    cidr_string = (char *) luaL_checkstring(L, 2);

    slash_location = strchr(cidr_string, '/');
    if(! slash_location) {
        return luaL_error(L, "Invalid CIDR string: %s", cidr_string);
    }

    cidr_string    = strdup(cidr_string);
    slash_location = strchr(cidr_string, '/');

    *slash_location = '\0';

    if(! inet_pton(AF_INET, cidr_string, &subnet)) {
        free(cidr_string);
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        lua_pushinteger(L, errno);
        return 3;
    }
    *slash_location = '/';

    /* XXX validate */
    mask_len = atoi(slash_location + 1);

    mask.s_addr = 0xffffffff;
    mask.s_addr >>= (32 - mask_len);

    lua_pushboolean(L, (address->s_addr & mask.s_addr) == (subnet.s_addr & mask.s_addr));
    return 1;
}

static void
push_ip_address(lua_State *L, struct in_addr *ip_address)
{
    struct in_addr *lua_ip_address;

    lua_ip_address = (struct in_addr *) lua_newuserdata(L, sizeof(struct in_addr));
    luaL_getmetatable(L, IP_ADDRESS_METATABLE);
    lua_setmetatable(L, -2);

    memcpy(lua_ip_address, ip_address, sizeof(struct in_addr));
}

static void
inject_network_info(autopilot_context *ap, struct autopilot_network_info *info)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    switch(info->state) {
        case AP_NETWORK_DISCONNECTED:
            autopilot_signal_fire(ap, "network/disconnected", 0);
            break;
        case AP_NETWORK_WIFI:
            lua_newtable(L);

            lua_pushboolean(L, 1);
            lua_setfield(L, -2, "is_wireless");

            lua_pushstring(L, info->wifi_info.ssid);
            lua_setfield(L, -2, "ssid");

            push_ip_address(L, &(info->wifi_info.ip_address));
            lua_setfield(L, -2, "address");

            autopilot_signal_fire(ap, "network/connected", 1);
            break;
        case AP_NETWORK_WIRED:
            lua_newtable(L);

            lua_pushboolean(L, 1);
            lua_setfield(L, -2, "is_wireless");

            push_ip_address(L, &(info->wifi_info.ip_address));
            lua_setfield(L, -2, "address");

            autopilot_signal_fire(ap, "network/connected", 1);
            break;
        default:
            autopilot_log_warn(ap, "Unknown AP network state: %d",
                info->state);
            break;
    }
}

static int
autopilot_network_source_init(autopilot_context *ap)
{
    lua_State *L;

    L = autopilot_get_lua(ap);

    luaL_newmetatable(L, IP_ADDRESS_METATABLE);

    lua_pushcfunction(L, lua_ip_address_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "__index");

    lua_pushcfunction(L, lua_ip_address_match);
    lua_setfield(L, -2, "match");

    lua_pop(L, 2);

    return 1;
}

struct autopilot_network_source_plugin autopilot_network_source_plugin = {
    .plugin = {
        .init     = autopilot_network_source_init,
        .finalize = NULL,
        .version  = {
            .major = 0,
            .minor = 1
        },
        .required_version = {
            .major = 0,
            .minor = 1
        },
    },

    .inject_network_info = inject_network_info
};
