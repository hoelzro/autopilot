#include <autopilot.h>
#include <autopilot/private.h>

#include <event.h>

int
autopilot_events_init(autopilot_context *ap)
{
    autopilot_log_info(ap, "initializing events...");

    ap->event_base = event_base_new();
    if(! ap->event_base) {
        autopilot_log_error(ap, "failed to initialize events!");
        return 0;
    }
    /* XXX bind event_* callbacks to our logging facility and such */
    autopilot_log_info(ap, "successfully initialized events!");
    return 1;
}

void
autopilot_events_run(autopilot_context *ap)
{
    int status;

    autopilot_log_info(ap, "running event loop");
    status = event_base_dispatch(ap->event_base);

    switch(status) {
        case -1: /* error */
            autopilot_log_error(ap, "Error setting up event system");
            break;
        case 1: /* no events */
            autopilot_log_error(ap, "No events registered with the event system");
            break;
        case 0: /* no problems */
            break;
    }
}

struct event_base *
autopilot_events_get_base(autopilot_context *ap)
{
    return ap->event_base;
}

int
autopilot_events_finalize(autopilot_context *ap)
{
    autopilot_log_info(ap, "tearing down events...");
    event_base_free(ap->event_base);
    autopilot_log_info(ap, "successfully tore down events!");
    return 1;
}
