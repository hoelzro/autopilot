#ifndef AP_EVENTS_H
#define AP_EVENTS_H

#include <autopilot/autopilot.h>

/** \defgroup EventLoop Event Loop
 * @{
 */

struct event_base;

/** \brief Get the event_base structure associated with this autopilot
 *         instance.
 *
 */

struct event_base *autopilot_events_get_base(autopilot_context *ap);

#endif

/** @} */
