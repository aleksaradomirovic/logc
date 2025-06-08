/*
 * Copyright 2025 Aleksa Radomirovic
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MODLOGC_H
#define _MODLOGC_H


#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Describes a log level for loggers
 * @param level log level (custom should in range 0 < level < 100000)
 * @param name log name for printing
 */
typedef struct {
    unsigned int level;
    const char *name;
} log_level_t;

/** Built-in log level for reporting program errors */
#define LOG_LEVEL_ERROR   (log_level_t) { 100, "error" }

/** Built-in log level for reporting program warnings */
#define LOG_LEVEL_WARNING (log_level_t) { 150, "warning" }

/** Built-in log level for general info */
#define LOG_LEVEL_INFO    (log_level_t) { 200, "info" }

/** Built-in log level for verbose info */
#define LOG_LEVEL_VERBOSE (log_level_t) { 250, "verbose" }

/** Built-in log level for debug info */
#define LOG_LEVEL_DEBUG   (log_level_t) { 300, "debug" }

/** Built-in log level for trace info */
#define LOG_LEVEL_TRACE   (log_level_t) { 350, "trace" }

/** Maximum value that a legal log level can be */
#define LOG_LEVEL_MAX_NUM 99999

/** Built-in default log level (platform/environment-defined) */
#define LOG_LEVEL_DEFAULT (log_level_t) { LOG_LEVEL_MAX_NUM + 1, "default" }

/** Built-in log level for disabling logging */
#define LOG_LEVEL_NONE    (log_level_t) { 0, "none" }

/**
 * An instance of a logger
 */
typedef struct logger * logger_t;

/**
 * Flag to for a logger to ignore overrides from the LOG_LEVEL
 * environment variable
 */
#define LOG_IGNORE_ENV (1 << 0)

/**
 * Create a new instance of a logger
 * @param output output stream for the logger to write to
 * @param flags flags to initialize the logger with
 * @return a new logger instance, or NULL if there was an error 
 * (errno set)
 * @note
 *  - output must be write-enabled (opened to "a", "a+", "w", or "w+")
 */
logger_t log_create(FILE *output, unsigned int flags);

/**
 * Destroy an instance of logger
 * @param logger logger to be destroyed
 * @return 0 for success; -1 for error (errno set)
 */
int log_destroy(logger_t logger);

/**
 * Set a logger's level number
 * @param logger logger to set
 * @param level log level number to set to
 * @return actual log level number the log was set to, which could be
 * unchanged if an illegal level was provided (errno set)
 */
unsigned int log_set_level(logger_t logger, unsigned int level);

/**
 * Set a logger's level number
 * @param logger logger to set
 * @return logger's curent level
 */
unsigned int log_get_level(logger_t logger);

/**
 * Print formatted string to log if log is of an equal or higher level
 * @param logger logger to print to
 * @param level message log level
 * @param format format string
 * @param ... format arguments
 * @return number of characters written to the log, or a negative value on error (errno set)
 */
int lprintf(logger_t logger, log_level_t level, const char *format, ...);

/**
 * Print formatted string to log if log is of an equal or higher level
 * @param logger logger to print to
 * @param level message log level
 * @param format format string
 * @param vlist format arguments list
 * @return number of characters written to the log, or a negative value on error (errno set)
 */
int vlprintf(logger_t logger, log_level_t level, const char *format, va_list vlist);

/**
 * Function template for printing the header before a log message
 * @param output stream to print to
 * @param level message log level
 * @return number of characters written to the log, or a negative value on error
 */
typedef int (log_header_t)(FILE *output, log_level_t level);

/**
 * Simplistic message header
 */
extern log_header_t log_header_simple;

extern log_header_t * const log_header_default;

/**
 * Set logger message header
 * @param logger logger to set the message header for
 * @param header message header to be set to
 * @return always returns 0 (success)
 */
int log_set_header(logger_t logger, log_header_t *header);

/**
 * Gets the log level number of the system/environment.
 * @return log level number
 */
unsigned int get_system_log_level();

/**
 * Standard open log attached to stderr
 */
extern logger_t stdlog;

/**
 * Attempt to initialize stdlog; may already be constructed on library init
 * @return 0 if successful or already open, -1 for error (errno set)
 */
int init_stdlog();


#ifdef __cplusplus
}
#endif


#endif
