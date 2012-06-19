#ifndef AP_AUTOPILOT_H
#define AP_AUTOPILOT_H

#define AUTOPILOT_VERSION_MAJOR 0
#define AUTOPILOT_VERSION_MINOR 1

/**
 * \defgroup Autopilot
 * @{
 */

/** \struct autopilot_context
 *  \brief The autopilot context object.
 *
 *  Autopilot functions all require an autopilot context
 *  object to be provided.  This is an opaque datatype.
 *
 */

struct autopilot_context;

typedef struct autopilot_context autopilot_context;

/** \brief A component initialization function.
 *
 *  You probably won't need this.
 *
 */

typedef int (*init_function)(autopilot_context *);

/** \brief A component finalization function.
 *
 *  You probably won't need this.
 *
 */

typedef int (*finalize_function)(autopilot_context *);

/** \brief A version datatype.
 *
 */

struct autopilot_version {
    /** \brief The major part of the version.
     */
    int major;

    /** \brief The minor part of the version.
     */
    int minor;
};

/** @} */

#endif
