#include <autopilot.h>
#include <event.h>
#include <stdlib.h>

/** This is an example plugin. */

static struct event *timer;

struct counter_wrapper {
    autopilot_context *ap;
    int counter;
};

static void
handle_timer(evutil_socket_t unused1, short unused2, void *udata)
{
    struct counter_wrapper *wrapper = (struct counter_wrapper *) udata;
    autopilot_context *ap           = wrapper->ap;
    lua_State *L                    = autopilot_get_lua(ap);

    lua_pushinteger(L, wrapper->counter++);
    autopilot_signal_fire(ap, "counter", 1);
}

static int
timer_initialize(autopilot_context *ap)
{
    struct event_base *ev = autopilot_events_get_base(ap);
    struct timeval timeout;
    struct counter_wrapper *wrapper;

    wrapper = malloc(sizeof(struct counter_wrapper));

    if(! wrapper) {
        return 0;
    }

    wrapper->ap      = ap;
    wrapper->counter = 0;

    timer = event_new(ev, -1, EV_PERSIST, handle_timer, wrapper);

    if(! timer) {
        free(wrapper);
        return 0;
    }

    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    /* XXX check error */
    if(event_add(timer, &timeout)) {
        event_free(timer);
        free(wrapper);
        return 0;
    }

    return 1;
}

static int
timer_finalize(autopilot_context *ap)
{
    /* XXX free the wrapper object */
    event_del(timer);
    event_free(timer);
    return 1;
}

autopilot_plugin autopilot_counter_plugin = {
    .init     = timer_initialize,
    .finalize = timer_finalize,
    .version  = {
        .major = 0,
        .minor = 1
    },
    .required_version  = {
        .major = 0,
        .minor = 1
    }
};
