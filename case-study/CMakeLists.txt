cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -std=c++20 -DJSON_MultipleHeaders=ON -Dflecs_EXPORTS")

project(university-impl)

set(FL_HOME ${PROJECT_SOURCE_DIR}/fuzzylite)

message("${FL_HOME}")
set(FL_INCLUDE_DIR ${FL_HOME})
set(FL_LIBRARY_DIR ${FL_HOME}/release/bin)

set(SOURCE_FILES src/world/world.cc
                 src/rules/rules.cc
                 src/helper/graph.cc
                 src/components/trace/trace.cc
                 src/components/needs/needs.cc
                 src/components/actions/actions.cc                 
                 src/components/calendar/calendar.cc
                 src/components/components/components.cc
                 src/components/environment/environment.cc)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${FL_INCLUDE_DIR})

find_library (FuzzyLiteLibrary fuzzylite-static HINTS ${FL_LIBRARY_DIR})
add_library(flecs src/flecs/flecs.c)

add_executable(university src/main.cc ${SOURCE_FILES})
target_include_directories(university PUBLIC "src/flecs" ${JSONCPP_INCLUDE_DIRS})

target_link_libraries(university PUBLIC flecs)
target_link_libraries(university LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(university PRIVATE ${FuzzyLiteLibrary})
