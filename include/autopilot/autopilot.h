#ifndef AP_AUTOPILOT_H
#define AP_AUTOPILOT_H

#define AUTOPILOT_VERSION_MAJOR 0
#define AUTOPILOT_VERSION_MINOR 1

struct autopilot_context;

typedef struct autopilot_context autopilot_context;

typedef int (*init_function)(autopilot_context *);
typedef int (*finalize_function)(autopilot_context *);

struct autopilot_version {
    int major;
    int minor;
};

#endif
