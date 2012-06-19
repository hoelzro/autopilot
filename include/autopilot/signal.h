#ifndef AP_SIGNALS_H
#define AP_SIGNALS_H

#include <autopilot/autopilot.h>

/** \defgroup Signals
 *  @{
 */

/** \brief Fires a signal into the autopilot instance.
 *
 *  When the signal is fired, the given number of values on the stack
 *  are passed to each handler function.  The values are cleared from the
 *  stack after all handlers are done firing.
 */

void autopilot_signal_fire(autopilot_context *ap, const char *name, int nargs_on_stack);

/** @} */

#endif
