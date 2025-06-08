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

#include <unistd.h>

static int output_is_tty(FILE *output) {
    int fd = fileno(output);
    if(fd == -1) {
        return 0;
    }

    return isatty(fd);
}

static const char * get_simple_color_code(log_level_t level) {
    if(level.level < LOG_LEVEL_WARNING.level) {
        return "1;31";
    } else if(level.level < LOG_LEVEL_INFO.level) {
        return "1;33";
    } else if(level.level < LOG_LEVEL_DEBUG.level) {
        return "1;37";
    } else if(level.level < LOG_LEVEL_TRACE.level) {
        return "1;90";
    } else {
        return "39";
    }
}

int log_header_simple(FILE *output, log_level_t level) {
    if(output_is_tty(output)) {
        return fprintf(output, "\e[%sm%s\e[0m: ", get_simple_color_code(level), level.name);
    } else {
        return fprintf(output, "%s: ", level.name);
    }
}

log_header_t * const log_header_default = log_header_simple;
