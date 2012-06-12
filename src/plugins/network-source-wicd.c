#include <autopilot.h>
#include <autopilot/plugin/dbus.h>
#include <autopilot/plugin/network-source.h>
#include <dbus/dbus.h>
#include <stdint.h>
#include <string.h>

#define WICD_STATUS_CHANGE "StatusChanged"
#define WICD_DESTINATION "org.wicd.daemon"
#define WICD_PATH "/org/wicd/daemon"
#define WICD_INTERFACE "org.wicd.daemon"

#define DBUS_FILTER "type='signal',interface=" WICD_INTERFACE ",member=" WICD_STATUS_CHANGE

#define PROBE_METHOD "GetConnectionStatus"
#define PROBE_TIMEOUT (5 * 1000)

enum wicd_state {
    WICD_DISCONNECTED,
    WICD_CONNECTING,
    WICD_CONNECTED_WIFI,
    WICD_CONNECTED_WIRED,
    WICD_SUSPENDED
};

/* XXX DON'T DO THIS! */
static DBusConnection *connection = NULL;
static uint32_t current_state     = WICD_DISCONNECTED - 1;

static int
populate_info_from_message(DBusMessage *message, struct autopilot_network_info *info, autopilot_context *ap)
{
    DBusMessageIter iter;
    uint32_t state;

    dbus_message_iter_init(message, &iter);

    if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRUCT) {
        dbus_message_iter_recurse(&iter, &iter);
    }

    dbus_message_iter_get_basic(&iter, &state);
    dbus_message_iter_next(&iter);

    if(state == current_state) {
        autopilot_log_debug(ap, "network state has not changed; skipping this event");
        return 0;
    }

    switch(state) {
        case WICD_DISCONNECTED:
            info->state   = AP_NETWORK_DISCONNECTED;
            current_state = state;
            break;
        case WICD_CONNECTED_WIFI:
            {
                DBusMessageIter array_iter;
                DBusMessageIter variant_iter;
                const char *address_str;

                info->state = AP_NETWORK_WIFI;

                dbus_message_iter_recurse(&iter, &array_iter);

                if(dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_VARIANT) {
                    dbus_message_iter_recurse(&array_iter, &variant_iter);
                    dbus_message_iter_get_basic(&variant_iter, &address_str);
                } else {
                    dbus_message_iter_get_basic(&array_iter, &address_str);
                }

                /* XXX check return */
                inet_pton(AF_INET, address_str, &(info->wifi_info.ip_address));

                dbus_message_iter_next(&array_iter);

                if(dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_VARIANT) {
                    dbus_message_iter_recurse(&array_iter, &variant_iter);
                    dbus_message_iter_get_basic(&variant_iter, &(info->wifi_info.ssid));
                } else {
                    dbus_message_iter_get_basic(&array_iter, &(info->wifi_info.ssid));
                }

                current_state = state;

                /* XXX copy SSID memory? */

                break;
            }
        case WICD_CONNECTED_WIRED:
            {
                DBusMessageIter array_iter;
                DBusMessageIter variant_iter;
                const char *address_str;

                info->state = AP_NETWORK_WIRED;

                dbus_message_iter_recurse(&iter, &array_iter);

                if(dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_VARIANT) {
                    dbus_message_iter_recurse(&array_iter, &variant_iter);
                    dbus_message_iter_get_basic(&variant_iter, &address_str);
                } else {
                    dbus_message_iter_get_basic(&array_iter, &address_str);
                }

                /* XXX check return */
                inet_pton(AF_INET, address_str, &(info->wired_info.ip_address));

                current_state = state;

                break;
            }
        case WICD_CONNECTING:
        case WICD_SUSPENDED:
            /* ignoring these */
            return 0;
        default:
            autopilot_log_warn(ap, "Unknown Wicd state: %lu", state);
            return 0;
    }

    return 1;
}

static DBusHandlerResult
handle_dbus_message(DBusConnection *connection, DBusMessage *message, void *udata)
{
    const char *member;
    autopilot_context *ap;
    struct autopilot_network_source_plugin *network_source_plugin;
    struct autopilot_network_info info;

    member = dbus_message_get_member(message);

    autopilot_log_debug(ap, "received D-Bus message: %s", member);

    /* we get other signals, like NameAcquired */
    if(strcmp(member, WICD_STATUS_CHANGE)) {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    ap                    = (autopilot_context *) udata;
    network_source_plugin = (struct autopilot_network_source_plugin *) autopilot_plugin_get(ap, "network-source");

    if(populate_info_from_message(message, &info, ap)) {
        network_source_plugin->inject_network_info(ap, &info);
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static int
autopilot_network_source_wicd_finalize(autopilot_context *ap)
{
    dbus_connection_close(connection);
    dbus_connection_unref(connection);
    return 1;
}

/* XXX how do we handle timeout? */
static void
handle_probe_reply(DBusPendingCall *pending, void *udata)
{
    DBusMessage *message;
    DBusMessageIter iter;
    DBusMessageIter struct_iter;
    autopilot_context *ap;
    struct autopilot_network_info info;
    struct autopilot_network_source_plugin *network_source_plugin;

    ap = (autopilot_context *) udata;
    network_source_plugin = (struct autopilot_network_source_plugin *) autopilot_plugin_get(ap, "network-source");

    message = dbus_pending_call_steal_reply(pending);
    dbus_message_iter_init(message, &iter);
    dbus_message_iter_recurse(&iter, &struct_iter);

    populate_info_from_message(message, &info, ap);

    dbus_message_iter_get_basic(&struct_iter, &current_state);

    network_source_plugin->inject_network_info(ap, &info);

    dbus_pending_call_unref(pending);
    dbus_message_unref(message);
}

static void
get_current_network_state(autopilot_context *ap, DBusConnection *connection)
{
    DBusMessage *message = NULL;
    DBusPendingCall *reply;
    dbus_bool_t result;

    message = dbus_message_new_method_call(WICD_DESTINATION, WICD_PATH,
        WICD_INTERFACE, PROBE_METHOD);

    if(! message) {
        autopilot_log_error(ap, "Unable to allocate message");
        return;
    }

    result = dbus_connection_send_with_reply(connection, message, &reply, PROBE_TIMEOUT);

    dbus_message_unref(message);

    if(! result) {
        autopilot_log_error(ap, "Unable to inspect current network state");
        return;
    }
    dbus_pending_call_set_notify(reply, handle_probe_reply, ap, NULL);
}

/* XXX IPv6 support? */
static int
autopilot_network_source_wicd_init(autopilot_context *ap)
{
    struct autopilot_dbus_plugin *dbus_plugin;
    DBusError error;

    dbus_plugin = (struct autopilot_dbus_plugin *) autopilot_plugin_get(ap, "dbus");

    dbus_error_init(&error);
    autopilot_log_debug(ap, "Acquiring D-Bus connection");
    connection = dbus_plugin->get_connection(ap, DBUS_BUS_SYSTEM, &error);

    if(! connection) {
        autopilot_log_error(ap, "Unable to acquire D-Bus connection: %s\n",
            error.message);
        dbus_error_free(&error);
        return 0;
    }

    autopilot_log_debug(ap, "Adding D-Bus match expression");
    dbus_bus_add_match(connection, DBUS_FILTER, &error);
    if(dbus_error_is_set(&error)) {
        autopilot_log_error(ap, "Unable to add match to D-Bus connection: %s\n",
            error.message);
        dbus_error_free(&error);
        autopilot_network_source_wicd_finalize(ap);
        return 0;
    }
    dbus_error_free(&error);
    autopilot_log_debug(ap, "Adding D-Bus filter function");
    if(! dbus_connection_add_filter(connection, handle_dbus_message, ap, NULL)) {
        autopilot_log_error(ap, "Unable to add filter to D-Bus connection!");
        autopilot_network_source_wicd_finalize(ap);
        return 0;
    }

    autopilot_log_debug(ap, "Inspecting current network state");
    get_current_network_state(ap, connection);

    return 1;
}

autopilot_plugin autopilot_network_source_wicd_plugin = {
    .init     = autopilot_network_source_wicd_init,
    .finalize = autopilot_network_source_wicd_finalize,
    .version  = {
        .major = 0,
        .minor = 1
    },
    .required_version  = {
        .major = 0,
        .minor = 1
    }
};
