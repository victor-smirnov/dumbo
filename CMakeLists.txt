
# Copyright 2017 Victor Smirnov
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


cmake_minimum_required(VERSION 2.6)
project(fibers)

find_package(
    Boost
    1.63.0 
    REQUIRED
) 

set(LIBRARY_OUTPUT_PATH lib)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)

set_property (GLOBAL PROPERTY GLOBAL_SOURCES)

add_subdirectory(src/fiber)


get_property(GLOBAL_SRCS GLOBAL PROPERTY GLOBAL_SOURCES)

add_library(Dumbo ${GLOBAL_SRCS})

export(TARGETS Dumbo FILE ${LIBRARY_OUTPUT_PATH}/Dumbo.cmake)

add_executable(main main.cpp )
target_link_libraries(main boost_context Dumbo)

SET_TARGET_PROPERTIES(main PROPERTIES COMPILE_FLAGS "-std=c++14 -Wfatal-errors -DDUMBO_FIBERS_NO_ATOMICS")

add_subdirectory(tests/fiber)

install(TARGETS main RUNTIME DESTINATION bin)
