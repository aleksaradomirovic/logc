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

#include "logc.h"

logger_t logger;

static void testlog() {
    log_write_f(logger, LOG_LEVEL_TRACE, "trace debug message");
    log_write_f(logger, LOG_LEVEL_DEBUG, "debug message");
    log_write_f(logger, LOG_LEVEL_VERBOSE, "verbose info message");
    log_write_f(logger, LOG_LEVEL_INFO, "info message");
    log_write_f(logger, LOG_LEVEL_WARNING, "warning message");
    log_write_f(logger, LOG_LEVEL_ERROR, "error message");
    log_write_f(logger, LOG_LEVEL_FATAL, "fatal error message");
}

int main() {
    logger = log_create(LOG_LEVEL_TRACE, NULL, 0);
    if(logger == NULL) {
        return 1;
    }

    testlog();

    if(log_destroy(logger) != 0) {
        return 2;
    }

    return 0;
}
