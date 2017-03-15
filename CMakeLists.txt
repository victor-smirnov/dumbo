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
