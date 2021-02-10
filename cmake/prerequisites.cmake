
#cmake_minimum_required(VERSION 3.13)
#mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)

# saner control structure syntax
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)


#list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/Modules:${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}")
#message("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}") ###

# this feature has been moved to cxx-detect-compilers.cmake
# set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(default_build_type "Release")
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif ()
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  # In non-win32 debug build, debug_malloc is on by default
  option(USE_DEBUG_MALLOC "Building with memory leak detection capability." ON)
  option(DEBUG "Building with DEBUG Mode" ON)
else ()
  # In win32 or non-debug builds, debug_malloc is off by default
  option(USE_DEBUG_MALLOC "Building with memory leak detection capability." OFF)
  option(DEBUG "Building with NON-DEBUG Mode" OFF)
endif ()


# for compiling
option(ENABLE_CCACHE "enable ccache optimizations" ON)
if (ENABLE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if (CCACHE_PROGRAM)
    message(STATUS "Set up ccache ...")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  endif ()
endif ()


# for testing
set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS)
mark_as_advanced(UNIT_TEST_TARGETS)
#
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_AUTOMATE_TESTS "Enable automated tests at local" ON)
if ($ENV{CI_RUNNING})
  set(ENABLE_AUTOMATE_TESTS OFF)
endif ()
if (${ENABLE_TESTS})
  enable_testing()
  #include(CTest) # note: this adds a BUILD_TESTING which defaults to ON
endif ()

# for installing
#include(GNUInstallDirs)


if ((CMAKE_VERBOSE_DEBUG AND DEBUG) OR ($ENV{CI_RUNNING}))
  # Enable verbose output from Makefile builds.
  # This variable is a cache entry initialized (to FALSE) by the project() command.
  # Users may enable the option in their local build tree to get more verbose
  # output from Makefile builds and show each command line as it is launched.
  set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
  # Default value for POSITION_INDEPENDENT_CODE of targets.
  # This variable is used to initialize the POSITION_INDEPENDENT_CODE property
  # on all the targets. See that target property for additional information.
  # If set, it’s value is also used by the try_compile() command.
  set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL "ON")
  message("CMAKE_VERBOSE_DEBUG ON")
endif ()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)


include(options)
