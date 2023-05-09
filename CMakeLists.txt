cmake_minimum_required(VERSION 3.22)
project("tropical" VERSION 1.0 LANGUAGES CXX)

# Options
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(SRC_PATH "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include")

# Targets
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${INCLUDE_PATH}/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SRC_PATH}/*.cpp")
add_executable("tropical" ${HEADERS} ${SOURCES})
target_include_directories("tropical" PRIVATE ${INCLUDE_PATH})

# 3rd Party Libraries
# cxxopts
find_package(cxxopts CONFIG REQUIRED)
target_link_libraries("tropical" PRIVATE cxxopts::cxxopts)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries("tropical" PRIVATE fmt::fmt)

# moodycamel::ConcurrentQueue
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
target_link_libraries("tropical" PRIVATE unofficial::concurrentqueue::concurrentqueue)

# mood::readerwriterqueue
include(FetchContent)
FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY    https://github.com/cameron314/readerwriterqueue
  GIT_TAG           master
)
FetchContent_MakeAvailable(readerwriterqueue)
add_library("spscqueue" "${INCLUDE_PATH}/util/spsc_queue.hpp")
set_target_properties("spscqueue" PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries("tropical" PRIVATE readerwriterqueue)

# toml++
find_package(tomlplusplus CONFIG REQUIRED)
target_link_libraries("tropical" PRIVATE tomlplusplus::tomlplusplus)

# Precompiled Headers
target_precompile_headers("tropical" PRIVATE)

# CCache
find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
    message(STATUS "ccache found: ${MOLD_EXECUTABLE}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# Compiler flags
if(MSVC)
    # Compiling with Microsoft Visual C++.
    list(APPEND WARNING_FLAGS "/W4")
else()
    # Compiling with g++/clang++/etc.
    list(
        APPEND WARNING_FLAGS
        "-pedantic-errors"
        "-Wfatal-errors"
        "-Werror=pedantic"
        "-Wall"
        "-Wextra"
        "-Wdouble-promotion"
        "-Wconversion"
        "-Wsign-conversion"
    )
endif()
target_compile_options("tropical" PRIVATE ${WARNING_FLAGS})

# Mold Linker
find_program(MOLD_EXECUTABLE "mold")

if(MOLD_EXECUTABLE)
    message(STATUS "mold linker found: ${MOLD_EXECUTABLE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
else()
    message(STATUS "mold linker not found.")
endif()

# Linker Flags
set_target_properties("tropical" PROPERTIES LINK_FLAGS_RELEASE -s)
message(STATUS "Release build binary striping enabled.")

# Link-Time-Optimization
include(CheckIPOSupported)

check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT USE_LTO_ERROR)

if(LTO_SUPPORTED)
    message(STATUS "LTO enabled")
    set_property(TARGET "tropical" PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "LTO not supported")
endif()
