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
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static FILE * dup_output(FILE *old_output) {
    int old_fd = fileno(old_output);
    if(old_fd == -1) {
        return NULL;
    }

    int flags = fcntl(old_fd, F_GETFL);
    if(flags == -1) {
        return NULL;
    }

    int accflags = (flags & O_ACCMODE);
    if(accflags != O_WRONLY && accflags != O_RDWR) {
        errno = EBADF;
        return NULL;
    }

    int fd = dup(old_fd);
    if(fd == -1) {
        return NULL;
    }

    FILE *output = fdopen(fd, flags & O_APPEND ? "a" : "w");
    if(output == NULL) {
        int err = errno;
        close(fd);
        errno = err;
        return NULL;
    }

    errno = 0;
    if(setvbuf(output, NULL, _IOLBF, BUFSIZ) != 0) {
        int err = errno;
        if(err == 0) {
            err = EIO;
        }
        fclose(output);
        errno = err;
        return NULL;
    }

    return output;
}

logger_t log_create(FILE *output, unsigned int flags) {
    if(flags & (~LOG_SUPPORTED_FLAGS)) {
        errno = EINVAL;
        return NULL;
    }

    logger_t logger = malloc(sizeof(struct logger));
    if(logger == NULL) {
        return NULL;
    }

    errno = 0;
    if(mtx_init(&logger->write_lock, mtx_plain) != thrd_success) {
        if(errno == 0) {
            errno = EAGAIN;
        }
        return NULL;
    }

    FILE *logfile = dup_output(output);
    if(logfile == NULL) {
        free(logger);
        return NULL;
    }

    logger->level = LOG_LEVEL_DEFAULT_PLATFORM.level;
    logger->flags = flags;
    logger->output = logfile;
    logger->header = log_header_default;

    return logger;
}

int log_destroy(logger_t logger) {
    if(fclose(logger->output) != 0) {
        return -1;
    }

    mtx_destroy(&logger->write_lock);
    free(logger);
    return 0;
}

unsigned int log_set_level(logger_t logger, unsigned int level) {
    if(level > LOG_LEVEL_MAX_NUM) {
        if(level == LOG_LEVEL_DEFAULT.level) {
            level = LOG_LEVEL_DEFAULT_PLATFORM.level;
        } else {
            errno = EINVAL;
            return logger->level;
        }
    }

    logger->level = level;
    return log_get_level(logger);
}

unsigned int log_get_level(logger_t logger) {
    return logger->level;
}

int log_set_header(logger_t logger, log_header_t *header) {
    logger->header = header;
    return 0;
}
