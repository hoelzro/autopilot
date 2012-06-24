#ifndef AP_PLUGIN_H
#define AP_PLUGIN_H

#include <autopilot/autopilot.h>

/** \defgroup Plugins
 * @{
 *
 *  Defines the interface for manipulating plugins.
 *  Here's an example plugin:
 *
 *  \include counter.c
 */

 */

/** \brief A structure representing a plugin.
 *
 */

struct autopilot_plugin {
    /** \brief The plugin's initialization function. May be NULL. */
    init_function init;

    /** \brief The plugin's finalization function. May be NULL. */
    finalize_function finalize;

    /** \brief The plugin's version.  Required. */
    struct autopilot_version version;

    /** \brief The version of autopilot with which this plugin will work.
     *         Required.
     */
    struct autopilot_version required_version;
};

typedef struct autopilot_plugin autopilot_plugin;

/** \brief Loads and returns the plugin structure for a plugin.
 */

autopilot_plugin *autopilot_plugin_get(autopilot_context *ap,
    const char *plugin_name);

/** @} */

#endif
