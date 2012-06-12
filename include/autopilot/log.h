#ifndef AP_LOG_H
#define AP_LOG_H

#include <autopilot/autopilot.h>

#ifdef __GNUC__
# define FORMAT_LIKE(fmt_index, first_index) __attribute__((format(printf, fmt_index, first_index)))
#else
# define FORMAT_LIKE(unused1, unused2)
#endif

FORMAT_LIKE(4, 5)
void _autopilot_log_fatal(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

FORMAT_LIKE(4, 5)
void _autopilot_log_error(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

FORMAT_LIKE(4, 5)
void _autopilot_log_warn(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

FORMAT_LIKE(4, 5)
void _autopilot_log_info(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

FORMAT_LIKE(4, 5)
void _autopilot_log_debug(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

FORMAT_LIKE(4, 5)
void _autopilot_log_trace(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...);

#define autopilot_log_fatal(ap, ...)\
    _autopilot_log_fatal(ap, __FILE__, __LINE__, __VA_ARGS__)

#define autopilot_log_error(ap, ...)\
    _autopilot_log_error(ap, __FILE__, __LINE__, __VA_ARGS__)

#define autopilot_log_warn(ap, ...)\
    _autopilot_log_warn(ap, __FILE__, __LINE__, __VA_ARGS__)

#define autopilot_log_info(ap, ...)\
    _autopilot_log_info(ap, __FILE__, __LINE__, __VA_ARGS__)

#define autopilot_log_debug(ap, ...)\
    _autopilot_log_debug(ap, __FILE__, __LINE__, __VA_ARGS__)

#define autopilot_log_trace(ap, ...)\
    _autopilot_log_trace(ap, __FILE__, __LINE__, __VA_ARGS__)

#endif
