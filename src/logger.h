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

#pragma once

#include "modlogc.h"

#include <threads.h>

#define LOG_LEVEL_DEFAULT_PLATFORM LOG_LEVEL_INFO

#define LOG_SUPPORTED_FLAGS (LOG_IGNORE_ENV)

struct logger {
    unsigned int level;
    unsigned int flags;

    FILE *output;
    log_header_t *header;

    mtx_t write_lock;
};
