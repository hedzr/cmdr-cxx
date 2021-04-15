
#cmake_minimum_required(VERSION 3.13)
#mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)

# saner control structure syntax
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)


# /usr/local/lib/cmake
# /usr/local/share/cmake/Modules
# /usr/share/cmake/Modules
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
endif ()
if (NOT CMAKE_CONFIGURATION_TYPES)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
  # set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  # In non-win32 debug build, debug_malloc is on by default
  option(USE_DEBUG_MALLOC "Building with memory leak detection capability." ON)
  option(DEBUG "Building with DEBUG Mode" ON)
  set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
else ()
  # In win32 or non-debug builds, debug_malloc is off by default
  option(USE_DEBUG_MALLOC "Building with memory leak detection capability." OFF)
  option(DEBUG "Building with NON-DEBUG Mode" OFF)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
    set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "" FORCE)
  elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_NAME "rel" CACHE STRING "release mode" FORCE)
    set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
  elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_BUILD_NAME "rms" CACHE STRING "min-size release mode" FORCE)
    set(CMAKE_MINSIZEREL_POSTFIX "ms" CACHE STRING "" FORCE)
  elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_BUILD_NAME "rwd" CACHE STRING "release mode with debug info" FORCE)
    set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
  endif ()
endif ()
mark_as_advanced(CMAKE_BUILD_NAME)


#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES
#                      DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
#                      RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX}
#                      MINSIZEREL_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX})


# ############################## for compiling

#
# CPU bits
#

option(ENABLE_CCACHE "enable ccache optimizations" ON)
if (ENABLE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if (CCACHE_PROGRAM)
    message(STATUS "Set up ccache ...")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  endif ()
endif ()

#
# CPU bits
#

if (CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
  set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
  #set(CPU_ARCH_NAME "x86_64" CACHE STRING "ARCH x86_64" FORCE)
  set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
else ()
  set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
  set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
endif ()


option(ENABLE_HICC_CLI_APP "Enable hicc cli app" ON)


# ############################## for testing
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

# ############################## for installing
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


# ############################## include .options.cmake
include(options)
