#ifndef AP_SIGNALS_H
#define AP_SIGNALS_H

#include <autopilot/autopilot.h>

void autopilot_signal_fire(autopilot_context *ap, const char *name, int nargs_on_stack);

#endif
