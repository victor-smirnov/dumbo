cmake_minimum_required(VERSION 2.6)
project(fibers)

find_package(
    Boost
    1.63.0 
    REQUIRED
) 

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)

set_property (GLOBAL PROPERTY GLOBAL_SOURCES)

add_subdirectory(src)

get_property(GLOBAL_SRCS GLOBAL PROPERTY GLOBAL_SOURCES)


add_executable(dumbo main.cpp ${GLOBAL_SRCS})
target_link_libraries(dumbo boost_context)

SET_TARGET_PROPERTIES(dumbo PROPERTIES COMPILE_FLAGS "-std=c++14 -Wfatal-errors")


install(TARGETS dumbo RUNTIME DESTINATION bin)
