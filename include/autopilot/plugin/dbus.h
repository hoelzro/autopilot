#ifndef AP_PLUGIN_DBUS_H
#define AP_PLUGIN_DBUS_H

#include <autopilot.h>
#include <dbus/dbus.h>

struct autopilot_dbus_plugin {
    autopilot_plugin plugin;

    DBusConnection *(*get_connection)(autopilot_context *ap, DBusBusType type,
        DBusError *error);
};

#endif
