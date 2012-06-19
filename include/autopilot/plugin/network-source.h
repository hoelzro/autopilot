#ifndef AP_PLUGIN_NETWORK_SOURCE_H
#define AP_PLUGIN_NETWORK_SOURCE_H

#include <autopilot.h>
#include <arpa/inet.h>

/** \defgroup NetworkSourcePlugin Network Source Plugin
 *  @{
 */

/** \brief A structure representing the current network connection state.
 */

struct autopilot_network_info {
    /** \brief The network state. */
    enum {
        AP_NETWORK_DISCONNECTED,
        AP_NETWORK_WIFI,
        AP_NETWORK_WIRED
    } state;

    union {
        /** \brief Wireless connection info.
         *
         *         The contents of this struct can only be considered valid if
         *         the state is equal to AP_NETWORK_WIFI.
         */
        struct {
            struct in_addr ip_address;
            const char *ssid;
        } wifi_info;

        /** \brief Wired connection info.
         *
         *         The contents of this struct can only be considered valid if
         *         the state is equal to AP_NETWORK_WIRED.
         */
        struct {
            struct in_addr ip_address;
        } wired_info;
    };
};

/** \brief A structure for the network source plugin.
 */

struct autopilot_network_source_plugin {
    autopilot_plugin plugin;

    /** \brief Fires a signal corresponding to the information in the given
     *         network info struct.
     */
    void (*inject_network_info)(autopilot_context *,
        struct autopilot_network_info *);
};

/** @} */

#endif
