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

#include <stdlib.h>

logger_t stdlog = NULL;

__attribute__((destructor))
static void fini_stdlog() {
    if(stdlog != NULL) {
        if(log_destroy(stdlog) == 0) {
            stdlog = NULL;
        }
    }
}

__attribute__((constructor))
int init_stdlog() {
    if(stdlog != NULL) {
        return 0;
    }

    stdlog = log_create(stderr, 0);
    if(stdlog == NULL) {
        return -1;
    }

    atexit(fini_stdlog);

    return 0;
}
