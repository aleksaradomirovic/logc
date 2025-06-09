# Copyright 2025 Aleksa Radomirovic
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.25)
project(modlogc LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
    EasyConfig
    GIT_REPOSITORY https://github.com/aleksaradomirovic/easyconfig.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(EasyConfig)

easyconfig_project(BUILD_SHARED_LIBS ON)

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")

add_library(
    modlogc
        src/env.c
        src/header.c
        src/logger.c
        src/stdlog.c
        src/write.c
)
target_include_directories(
    modlogc
    PUBLIC
        include   
)
target_link_libraries(
    modlogc
    PRIVATE
        pthread
)

link_libraries(modlogc)
add_executable(
    basic
    EXCLUDE_FROM_ALL
        example/basic.c
)
