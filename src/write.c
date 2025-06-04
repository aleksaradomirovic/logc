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

#include "logger.h"

#include <errno.h>
#include <stdarg.h>

static const char * get_log_level_str(log_level_t level) {
    switch(level) {
        case LOG_LEVEL_FATAL:   return "fatal";
        case LOG_LEVEL_ERROR:   return "error";
        case LOG_LEVEL_WARNING: return "warning";
        case LOG_LEVEL_INFO:    return "info";
        case LOG_LEVEL_VERBOSE: return "verbose";
        case LOG_LEVEL_DEBUG:   return "debug";
        case LOG_LEVEL_TRACE:   return "trace";
    }

    return "(unknown)";
}

static const char * get_log_level_color(log_level_t level) {
    switch(level) {
        case LOG_LEVEL_FATAL:
        case LOG_LEVEL_ERROR:   return "1;31";
        case LOG_LEVEL_WARNING: return "1;33";
        case LOG_LEVEL_INFO:    return "1;39";
        case LOG_LEVEL_VERBOSE: return "39";
        case LOG_LEVEL_DEBUG:   return "1;90";
        case LOG_LEVEL_TRACE:   return "90";
    }

    return "0";
}

static int locked_log_write_f(logger_t logger, int tty, log_level_t level, const char *fmt, va_list list) {
    errno = 0;
    int status;
    if(tty) {
        status = fprintf(logger->output, "\e[%sm%s\e[0m: ", get_log_level_color(level), get_log_level_str(level));
    } else {
        status = fprintf(logger->output, "%s: ", get_log_level_str(level));
    }
    if(status < 0) {
        if(errno == 0) {
            errno = EIO;
        }
        return -1;
    }
    
    errno = 0;
    int count = vfprintf(logger->output, fmt, list);
    if(count < 0) {
        if(errno == 0) {
            errno = EIO;
        }
        return count;
    }

    errno = 0;
    if(fprintf(logger->output, "\n") < 0) {
        if(errno == 0) {
            errno = EIO;
        }
        return -1;
    }

    return count;
}

int lprintf(logger_t logger, log_level_t level, const char *fmt, ...) {
    log_level_t max_level;
    if((logger->flags & LOG_FLAG_OVERRIDE_ENV_LEVEL) || env_log_level < 0) {
        max_level = logger->level;
    } else {
        max_level = env_log_level;
    }

    if(level > max_level) {
        return 0;
    }

    int err = errno;
    int tty = log_is_tty(logger);
    errno = err;

    if(pthread_mutex_lock(&logger->write_lock) != 0) {
        return -1;
    }

    va_list args;
    va_start(args, fmt);
    int count = locked_log_write_f(logger, tty, level, fmt, args);
    va_end(args);

    if(pthread_mutex_unlock(&logger->write_lock) != 0) {
        return -1;
    }

    return count;
}
