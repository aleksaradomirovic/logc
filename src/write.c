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
#include <stdckdint.h>

#define LINE_SEP "\n"

static int check_should_print(logger_t logger, log_level_t level) {
    unsigned int log_level = get_system_log_level();
    if(logger->level > log_level || ((logger->flags) & LOG_IGNORE_ENV)) {
        log_level = logger->level;
    }

    return (level.level <= log_level);
}

static int vlprintf_locked(logger_t logger, log_level_t level, const char *format, va_list vlist) {
    int count = 0, sum;
    if(logger->header != NULL) {
        errno = 0;
        sum = logger->header(logger->output, level);
        if(sum < 0) {
            if(errno == 0) {
                errno = EIO;
            }
            return sum;
        }
        if(ckd_add(&count, count, sum)) {
            errno = EOVERFLOW;
            return -1;
        }
    }

    errno = 0;
    sum = vfprintf(logger->output, format, vlist);
    if(sum < 0) {
        if(errno == 0) {
            errno = EIO;
        }
        return sum;
    }
    if(ckd_add(&count, count, sum)) {
        errno = EOVERFLOW;
        return -1;
    }

    errno = 0;
    sum = fprintf(logger->output, LINE_SEP);
    if(sum < 0) {
        if(errno == 0) {
            errno = EIO;
        }
        return sum;
    }
    if(ckd_add(&count, count, sum)) {
        errno = EOVERFLOW;
        return -1;
    }

    return count;
}

int vlprintf(logger_t logger, log_level_t level, const char *format, va_list vlist) {
    if(logger == NULL) {
        errno = EFAULT;
        return -1;
    }

    if(!check_should_print(logger, level)) {
        return 0;
    }

    errno = 0;
    if(mtx_lock(&logger->write_lock) != thrd_success) {
        if(errno == 0) {
            errno = EAGAIN;
        }
        return -1;
    }

    int count = vlprintf_locked(logger, level, format, vlist);
    int err = errno;

    errno = 0;
    if(mtx_unlock(&logger->write_lock) != thrd_success) {
        if(errno == 0) {
            errno = EAGAIN;
        }
        return -1;
    }

    if(count < 0) {
        errno = err;
    }
    return count;
}

int lprintf(logger_t logger, log_level_t level, const char *format, ...) {
    va_list vlist;
    va_start(vlist, format);

    int count = vlprintf(logger, level, format, vlist);

    int err = errno;
    va_end(vlist);
    errno = err;

    return count;
}
