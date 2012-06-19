#ifndef AP_PLUGIN_DBUS_H
#define AP_PLUGIN_DBUS_H

#include <autopilot.h>
#include <dbus/dbus.h>

/** \defgroup DBusPlugin D-Bus Plugin
 *  @{
 */

/** \brief The plugin structure for the D-Bus plugin.
 */

struct autopilot_dbus_plugin {
    autopilot_plugin plugin;

    /** \brief Gets a D-Bus connection.  The connection's event loop functions
     *         are bound to the event loop of the given autopilot instance.
     */
    DBusConnection *(*get_connection)(autopilot_context *ap, DBusBusType type,
        DBusError *error);
};

/** @} */

#endif
