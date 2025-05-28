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

int log_set_level(logger_t logger, log_level_t level) {
    if(level == 0) {
        level = LOG_LEVEL_INFO;
    }

    logger->level = level;
    return level;
}

log_level_t log_get_level(logger_t logger) {
    return logger->level;
}

int log_set_output(logger_t logger, FILE *arg_file) {
    if(logger->output != NULL) {
        if(fclose(logger->output) != 0) {
            return -1;
        }
    }

    if(arg_file == NULL) {
        arg_file = stderr;
    }

    int arg_fd = fileno(arg_file);
    if(arg_fd == -1) {
        return -1;
    }

    int flags = fcntl(arg_fd, F_GETFL);
    if(flags == -1) {
        return -1;
    }

    int accmode = flags & O_ACCMODE;
    if(accmode != O_WRONLY && accmode != O_RDWR) {
        errno = EBADF;
        return -1;
    }

    int log_fd = dup(arg_fd);
    if(log_fd == -1) {
        return -1;
    }

    FILE *log_file = fdopen(log_fd, (flags & O_APPEND) ? "a" : "w");
    if(log_file == NULL) {
        int err = errno;
        close(log_fd);
        errno = err;
        return -1;
    }

    errno = 0;
    if(setvbuf(log_file, NULL, _IOLBF, BUFSIZ) != 0) {
        int err = errno;
        if(err == 0) {
            err = EIO;
        }
        fclose(log_file);
        errno = err;
        return -1;
    }

    logger->output = log_file;
    return 0;
}

unsigned int log_set_flags(logger_t logger, unsigned int flags) {
    logger->flags = flags;
    return flags;
}

unsigned int log_get_flags(logger_t logger) {
    return logger->flags;
}

logger_t log_create(log_level_t level, FILE *output, unsigned int flags) {
    logger_t logger = malloc(sizeof(struct logger));
    if(logger == NULL) {
        return NULL;
    }

    if(pthread_mutex_init(&logger->write_lock, NULL) != 0) {
        free(logger);
        return NULL;
    }

    log_set_level(logger, level);
    log_set_flags(logger, flags);

    logger->output = NULL;
    if(log_set_output(logger, output) != 0) {
        free(logger);
        return NULL;
    }

    return logger;
}

int log_destroy(logger_t logger) {
    if(fclose(logger->output) != 0) {
        return -1;
    }

    if(pthread_mutex_destroy(&logger->write_lock) != 0) {
        return -1;
    }

    free(logger);
    return 0;
}

int log_is_tty(logger_t logger) {
    int fd = fileno(logger->output);
    if(fd == -1) {
        return 0;
    }

    return isatty(fd);
}
