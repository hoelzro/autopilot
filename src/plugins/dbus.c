#include <autopilot.h>
#include <autopilot/plugin/dbus.h>

#include <dbus/dbus.h>
#include <event.h>

static void
invoke_dbus_watch(evutil_socket_t fd, short flags, void *udata)
{
    DBusWatch *watch;
    dbus_bool_t result;
    unsigned int dbus_flags;

    watch = (DBusWatch *) udata;

    dbus_flags = ((flags & EV_READ)  ? DBUS_WATCH_READABLE : 0) |
                 ((flags & EV_WRITE) ? DBUS_WATCH_WRITABLE : 0);

    result = dbus_watch_handle(watch, dbus_flags);
}

static dbus_bool_t
add_dbus_event(DBusWatch *watch, void *data)
{
    struct event *event;

    event = dbus_watch_get_data(watch);

    if(! event) {
        struct event_base *ev;
        unsigned int dbus_flags;
        short ev_flags;
        int fd;

        fd         = dbus_watch_get_unix_fd(watch);
        dbus_flags = dbus_watch_get_flags(watch);

        ev_flags = ((dbus_flags & DBUS_WATCH_READABLE) ? EV_READ  : 0) |
                   ((dbus_flags & DBUS_WATCH_WRITABLE) ? EV_WRITE : 0) |
                   EV_PERSIST;

        ev = (struct event_base *) data;

        event = event_new(ev, fd, ev_flags, invoke_dbus_watch, watch);
        if(! event) {
            return FALSE;
        }

        dbus_watch_set_data(watch, event, NULL);
    }

    if(dbus_watch_get_enabled(watch)) {
        if(event_add(event, NULL)) {
            event_free(event);
            dbus_watch_set_data(watch, NULL, NULL);
            return FALSE;
        }
    }
    return TRUE;
}

static void
remove_dbus_event(DBusWatch *watch, void *data)
{
    struct event *event;

    event = dbus_watch_get_data(watch);
    event_del(event);
    event_free(event);
}

static void
toggle_dbus_event(DBusWatch *watch, void *data)
{
    struct event *event;

    event = dbus_watch_get_data(watch);

    if(dbus_watch_get_enabled(watch)) {
        event_del(event);
    } else {
        add_dbus_event(watch, data);
    }
}

static void
invoke_dbus_timeout(evutil_socket_t unused1, short unused2, void *udata)
{
    DBusTimeout *timeout;
    dbus_bool_t result;

    timeout = (DBusTimeout *) udata;

    result = dbus_timeout_handle(timeout);
}

static dbus_bool_t
add_dbus_timeout(DBusTimeout *timeout, void *data)
{
    struct event *event;

    event = dbus_timeout_get_data(timeout);

    if(! event) {
        struct event_base *ev;

        ev = (struct event_base *) data;

        event = event_new(ev, -1, EV_PERSIST, invoke_dbus_timeout, timeout);
        if(! event) {
            return FALSE;
        }

        dbus_timeout_set_data(timeout, event, NULL);
    }

    if(dbus_timeout_get_enabled(timeout)) {
        int interval = dbus_timeout_get_interval(timeout);
        struct timeval tv;

        tv.tv_sec  = (interval / 1000);
        tv.tv_usec = (interval % 1000) * 1000;

        if(event_add(event, &tv)) {
            event_free(event);
            dbus_timeout_set_data(timeout, NULL, NULL);
            return FALSE;
        }
    }
    return TRUE;
}

static void
remove_dbus_timeout(DBusTimeout *timeout, void *data)
{
    struct event *event;

    event = dbus_timeout_get_data(timeout);
    event_del(event);
    event_free(event);
}

static void
toggle_dbus_timeout(DBusTimeout *timeout, void *data)
{
    struct event *event;

    event = dbus_timeout_get_data(timeout);

    if(dbus_timeout_get_enabled(timeout)) {
        event_del(event);
    } else {
        add_dbus_timeout(timeout, data);
    }
}

static void
handle_dispatch_status_change(DBusConnection *connection, DBusDispatchStatus status, void *data)
{
    struct event *dispatch_event;
    struct timeval dispatch_timeout;

    dispatch_timeout.tv_sec  = 1;
    dispatch_timeout.tv_usec = 0;

    dispatch_event = (struct event *) data;

    if(status == DBUS_DISPATCH_DATA_REMAINS) {
        event_add(dispatch_event, &dispatch_timeout);
    } else {
        event_del(dispatch_event);
    }
}

static void
handle_dispatch_event(evutil_socket_t unused1, short unused2, void *udata)
{
    DBusConnection *connection = (DBusConnection *) udata;
    DBusDispatchStatus status  = dbus_connection_get_dispatch_status(connection);

    if(status == DBUS_DISPATCH_DATA_REMAINS) {
        dbus_connection_dispatch(connection);
    }
}

static void
handle_wakeup(void *data)
{
    struct event *event;

    event = (struct event *) data;

    event_active(event, 0, 0);
}

static DBusConnection *
get_connection(autopilot_context *ap, DBusBusType type, DBusError *error)
{
    DBusConnection *connection;

    connection = dbus_bus_get_private(type, error);

    if(connection) {
        struct event_base *ev;
        struct event *dispatch_event = NULL;
        struct timeval dispatch_timeout;

        ev = autopilot_events_get_base(ap);

        dispatch_timeout.tv_sec  = 1;
        dispatch_timeout.tv_usec = 0;

        /* XXX clean up */
        dispatch_event = event_new(ev, -1, EV_PERSIST, handle_dispatch_event, connection);
        event_add(dispatch_event, &dispatch_timeout);

        if(! dbus_connection_set_watch_functions(connection, add_dbus_event, remove_dbus_event, toggle_dbus_event, ev, NULL)) {
            return NULL;
        }
        if(! dbus_connection_set_timeout_functions(connection, add_dbus_timeout, remove_dbus_timeout, toggle_dbus_timeout, ev, NULL)) {
            return NULL;
        }
        dbus_connection_set_dispatch_status_function(connection, handle_dispatch_status_change, dispatch_event, NULL);
        dbus_connection_set_wakeup_main_function(connection, handle_wakeup, dispatch_event, NULL);

    }
    return connection;
}

struct autopilot_dbus_plugin autopilot_dbus_plugin = {
    .plugin = {
        .init     = NULL,
        .finalize = NULL,
        .version  = {
            .major = 0,
            .minor = 1
        },
        .required_version  = {
            .major = 0,
            .minor = 1
        }
    },

    .get_connection = get_connection
};
