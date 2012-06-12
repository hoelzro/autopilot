#ifndef AP_PRIVATE_EVENTS_H
#define AP_PRIVATE_EVENTS_H

int autopilot_events_init(autopilot_context *ap);

void autopilot_events_run(autopilot_context *ap);

int autopilot_events_finalize(autopilot_context *ap);

#endif
