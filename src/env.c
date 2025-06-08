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

static unsigned int env_log_level = LOG_LEVEL_MAX_NUM + 1;

static int get_env_log_level() {
    const char *env_level_str = getenv("LOG_LEVEL");
    if(env_level_str == NULL) {
        return -1;
    }

    const char *end;
    unsigned long level = strtoul(env_level_str, (char **) &end, 10);
    if(level != 0 || end > env_level_str) {
        if(level > LOG_LEVEL_MAX_NUM) {
            return -1;
        }

        while(1) {
            if(*end == '\0') {
                env_log_level = level;
                return 0;
            }
            if(!isspace(*end)) {
                break;
            }
            end++;
        }
    }

    char cmp_str[strlen(env_level_str) + 1];
    strcpy(cmp_str, env_level_str);
    for(size_t i = 0; cmp_str[i] != '\0'; i++) {
        cmp_str[i] = tolower(cmp_str[i]);
    }

    log_level_t cmp[] = {
        LOG_LEVEL_NONE,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_VERBOSE,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_TRACE,
        { 0, NULL }
    };

    for(size_t i = 0;; i++) {
        log_level_t cmp_level = cmp[i];
        if(cmp_level.name == NULL) {
            break;
        }

        if(strcmp(env_level_str, cmp_level.name) == 0) {
            env_log_level = cmp_level.level;
            return 0;
        }
    }

    return -1;
}

unsigned int get_system_log_level() {
    if(env_log_level > LOG_LEVEL_MAX_NUM) {
        if(get_env_log_level() != 0) {
            env_log_level = LOG_LEVEL_DEFAULT_PLATFORM.level;
        }
    }

    return env_log_level;
}
