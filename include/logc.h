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

#ifndef _LOGC_H
#define _LOGC_H


#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _Nullable
#define _Nullable
#endif

typedef struct logger * logger_t;
typedef int log_level_t;

#define LOG_LEVEL_NONE    0x0f
#define LOG_LEVEL_FATAL   0x20
#define LOG_LEVEL_ERROR   0x21
#define LOG_LEVEL_WARNING 0x30
#define LOG_LEVEL_INFO    0x40
#define LOG_LEVEL_VERBOSE 0x41
#define LOG_LEVEL_DEBUG   0x50
#define LOG_LEVEL_TRACE   0x51
#define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO

#define LOG_FLAG_OVERRIDE_ENV_LEVEL (1 << 0)

logger_t log_create(log_level_t level, FILE *_Nullable output, unsigned int flags);
int log_destroy(logger_t);

unsigned int log_set_flags(logger_t, unsigned int flags);
unsigned int log_get_flags(logger_t);

int log_set_level(logger_t, log_level_t level);
log_level_t log_get_level(logger_t);

int log_set_output(logger_t, FILE *_Nullable output);

int log_is_tty(logger_t);

int lprintf(logger_t, log_level_t level, const char *fmt, ...);

#define LOG_LEVEL_ENV_KEY "LOG_LEVEL"

extern logger_t stdlog;

#ifdef __cplusplus
}
#endif


#endif
