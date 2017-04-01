
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


cmake_minimum_required(VERSION 3.1.0)
project(Dumbo)

option(BUILD_TESTS "Build tests" FALSE)

#set (BUILD_WINDOWS FALSE)
#set (BUILD_LINUX FALSE)
#set (BUILD_BSD FALSE)
#set (BUILD_DARWIN FALSE)

if (${CMAKE_SYSTEM_NAME} STREQUAL Windows) 
    set(BUILD_WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
    set (BUILD_BSD TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    set (BUILD_DARWIN TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    set (BUILD_LINUX TRUE)
else()
    message(FATAL_ERROR "Unsupported build environment: ${CMAKE_SYSTEM_NAME}")
endif()


set(CMAKE_CXX_STANDARD 14)

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED OFF)

find_package(
    Boost
    1.63.0 
    REQUIRED
) 


set(LIBRARY_OUTPUT_PATH lib)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)



if (BUILD_LINUX)
    set (DUMBO_COMPILE_FLAGS "-Wfatal-errors")
    set (DUMBO_LINK_FLAGS "-pthread")
else()
    set (DUMBO_COMPILE_FLAGS "-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS")  
    set (DUMBO_LINK_FLAGS "")
endif()


set_property (GLOBAL PROPERTY GLOBAL_SOURCES)

add_subdirectory(src/fiber)
add_subdirectory(src/reactor)


get_property(GLOBAL_SRCS GLOBAL PROPERTY GLOBAL_SOURCES)

add_library(Dumbo ${GLOBAL_SRCS})
set_target_properties(Dumbo PROPERTIES COMPILE_FLAGS "${DUMBO_COMPILE_FLAGS}")
export(TARGETS Dumbo FILE ${LIBRARY_OUTPUT_PATH}/Dumbo.cmake)

add_executable(main main.cpp )
SET_TARGET_PROPERTIES(main PROPERTIES COMPILE_FLAGS "${DUMBO_COMPILE_FLAGS}")
set_target_properties(main PROPERTIES LINK_FLAGS "${DUMBO_LINK_FLAGS}")
target_link_libraries(main Dumbo)

if (${BUILD_TESTS})
	add_subdirectory(tests/fiber)
	add_subdirectory(tests/disruptor)
	#add_subdirectory(tests/reactor)
endif()

#install(TARGETS main RUNTIME DESTINATION bin)
