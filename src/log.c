#include <autopilot/log.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define RED     "\e[31m"
#define GREEN   "\e[32m"
#define YELLOW  "\e[33m"
#define BLUE    "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN    "\e[36m"
#define WHITE   "\e[37m"

#define ON_BLACK   "\e[40m"
#define ON_RED     "\e[41m"
#define ON_GREEN   "\e[42m"
#define ON_YELLOW  "\e[43m"
#define ON_BLUE    "\e[44m"
#define ON_MAGENTA "\e[45m"
#define ON_CYAN    "\e[46m"
#define ON_WHITE   "\e[47m"

#define RESET      "\e[0m"
#define BRIGHT     "\e[1m"
#define DIM        "\e[2m"
#define UNDERSCORE "\e[4m"
#define BLINK      "\e[5m"
#define REVERSE    "\e[7m"
#define HIDDEN     "\e[8m"

#define FATAL_COLOR RED ON_YELLOW
#define ERROR_COLOR RED
#define WARN_COLOR  YELLOW
#define INFO_COLOR  BRIGHT GREEN
#define DEBUG_COLOR MAGENTA
#define TRACE_COLOR CYAN

#define PREFIX_FORMAT "%-25s %-5s "

#define log_it(level, color)\
    _log_it(level, color PREFIX_FORMAT, fmt, filename, args);

static void
_log_it(const char *level, const char *prefix, const char *format, const char *filename, va_list args)
{
    printf(prefix, filename, level);
    vprintf(format, args);

    if(format[strlen(format) - 1] != '\n') {
        printf(RESET "\n");
    }
}

void
_autopilot_log_fatal(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("FATAL", FATAL_COLOR);
}

void
_autopilot_log_error(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("ERROR", ERROR_COLOR);
}

void
_autopilot_log_warn(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("WARN", WARN_COLOR);
}

void
_autopilot_log_info(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("INFO", INFO_COLOR);
}

void
_autopilot_log_debug(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("DEBUG", DEBUG_COLOR);
}

void
_autopilot_log_trace(autopilot_context *ap, const char *filename, int line_number, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    log_it("TRACE", TRACE_COLOR);
}

int
autopilot_log_init(autopilot_context *ap)
{
    autopilot_log_info(ap, "successfully initialized logs!");
    return 1;
}

int
autopilot_log_finalize(autopilot_context *ap)
{
    autopilot_log_info(ap, "successfully tore down logs!");
    return 1;
}
