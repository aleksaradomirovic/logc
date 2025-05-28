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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

logger_t stdlog = NULL;

static log_level_t get_env_log_level() {
    char *env_str = getenv(LOG_LEVEL_ENV_KEY);
    if(env_str == NULL) {
        return -1;
    }

    char level_str[strlen(env_str) + 1];
    for(size_t i = 0;; i++) {
        if(env_str[i] == '\0') {
            level_str[i] = '\0';
            break;
        }

        level_str[i] = tolower(env_str[i]);
    }

    if(strcmp(level_str, "fatal") == 0) {
        return LOG_LEVEL_FATAL;
    } else if(strcmp(level_str, "error") == 0) {
        return LOG_LEVEL_ERROR;
    } else if(strcmp(level_str, "warning") == 0) {
        return LOG_LEVEL_WARNING;
    } else if(strcmp(level_str, "info") == 0) {
        return LOG_LEVEL_INFO;
    } else if(strcmp(level_str, "verbose") == 0) {
        return LOG_LEVEL_VERBOSE;
    } else if(strcmp(level_str, "debug") == 0) {
        return LOG_LEVEL_DEBUG;
    } else if(strcmp(level_str, "trace") == 0) {
        return LOG_LEVEL_TRACE;
    }

    return -1;
}

__attribute__((constructor))
static void init_stdlog() {
    env_log_level = get_env_log_level();
    stdlog = log_create(LOG_LEVEL_DEFAULT, stderr, 0);
}

__attribute__((destructor))
static void fini_stdlog() {
    if(stdlog != NULL) {
        log_destroy(stdlog);
    }
}

log_level_t env_log_level;
