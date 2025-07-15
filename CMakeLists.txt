# # TPL ## HEAD ##########
cmake_minimum_required(VERSION 3.11)

# ### INCLUDES ##########################################################
include(${CMAKE_SOURCE_DIR}/cmake/prerequisites.cmake)

# use_vcpkg()

# ### DEFINITIONS #######################################################
set(PROJECT_MACRO_NAME cmdr-cxx)
set(PROJECT_MACRO_SHORT_NAME cmdr)
set(PROJECT_MACRO_MID_NAME cmdr11)
set(PROJECT_MACRO_PREFIX "CMDR")
set_property(GLOBAL PROPERTY PROJECT_MAINTAINER "hedzr <hedzr@duck.com>")

# ### OPTIONS ###########################################################

# option(OK "PL-OK" ON)

# option(${PROJECT_MACRO_PREFIX}_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
# option(${PROJECT_MACRO_PREFIX}_BUILD_DOCS "Build Docs-OK" OFF)

# the following options are just used for executables
option(${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS "Enable Assertions" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
option(${PROJECT_MACRO_PREFIX}_UNIT_TEST "Enable the extra unit-tests" OFF)


# ### PROJECTS PREPARE ##################################################
if(ENABLE_TESTS)
    include(CTest)
endif()

find_package(Threads REQUIRED)

# ### PROJECTS ##########################################################
project(cmdr-cxx
	VERSION ${VERSION}
	DESCRIPTION "cmdr-cxx is a POSIX-compliant command-line arguments parser in C++, its part of cmdr series."
	LANGUAGES CXX)
debug_print_project_title()

set(VERSION_H_NAME "cmdr-version.hh")

# Enable C++ Standard 17/20 here. Change it on fist configuring, or update it with new value:
# cmake -S. -Bbuild -DCXX_STANDARD:STRING=20
# cmake -S. -Bbuild -DCXX_STANDARD:STRING=17
set(CXX_STANDARD 20 CACHE STRING "Define The C++ Standard, default is 20")
enable_cxx_standard(CXX_STANDARD)

# if(ENABLE_TESTS)
#     include(load-catch2)
# endif()

# # when using compiler with cmake multi-config feature, a special build type 'Asan' can be used for sanitizing test.
# enable_sanitizer_for_multi_config()

# # include(cxx-standard-def)
# include(cxx-detect-compilers)
# include(setup-build-env)

# debug_print_top_vars()

gen_versions(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX}
	${PROJECT_MACRO_SHORT_NAME}-version.hh
	${PROJECT_MACRO_SHORT_NAME}-config-base.hh
	${PROJECT_MACRO_SHORT_NAME}-${PROJECT_VERSION}
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
)

#
# add more subdirectories as submodules and sub-submodules HERE
#

# define libcmdr11 as a library
add_subdirectory(libs) # adds cmdr-cxx target, and others

if(${ENABLE_TESTS} OR(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
	add_subdirectory(tests)
endif()

if(${ENABLE_DOCS})
	add_subdirectory(docs)
	attach_doxygen_to(${PROJECT_MACRO_MID_NAME})
endif()

if(${ENABLE_EXAMPLES} OR(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
	add_subdirectory(examples)
endif()

# # TPL ## TAIL ##########
message(STATUS "tail of top level : Using C++ Standard ${CMAKE_CXX_STANDARD}")
apply_all_unit_tests(all_tests) # invoke CTest unittests automatically.
dump_feature_summary() # tail print to cmake-debug/features.log

# env var CTEST_OUTPUT_ON_FAILURE=1 to display ctest error verbosely.
# ctest cmdline options: --verbose or --extra-verbose or --verbose-on-failure
debug_print_value(DEBUG)
debug_print_value(BUILD_DOCUMENTATION)
