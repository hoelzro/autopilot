#ifndef AP_PLUGIN_NETWORK_SOURCE_H
#define AP_PLUGIN_NETWORK_SOURCE_H

#include <autopilot.h>
#include <arpa/inet.h>

struct autopilot_network_info {
    enum {
        AP_NETWORK_DISCONNECTED,
        AP_NETWORK_WIFI,
        AP_NETWORK_WIRED
    } state;

    union {
        struct {
            struct in_addr ip_address;
            const char *ssid;
        } wifi_info;

        struct {
            struct in_addr ip_address;
        } wired_info;
    };
};

struct autopilot_network_source_plugin {
    autopilot_plugin plugin;

    void (*inject_network_info)(autopilot_context *,
        struct autopilot_network_info *);
};

#endif
