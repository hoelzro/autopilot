#ifndef AP_EVENTS_H
#define AP_EVENTS_H

#include <autopilot/autopilot.h>

struct event_base;

struct event_base *autopilot_events_get_base(autopilot_context *ap);

#endif
