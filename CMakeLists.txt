# # TPL ## HEAD ##########
cmake_minimum_required(VERSION 3.9..)
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/addons
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules
	${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
	${CMAKE_SOURCE_DIR} # for .version.cmake & .options.cmake
	${CMAKE_MODULE_PATH})
include(prerequisites)

# include(dummy-project)
project(cmdr-cxx
	VERSION ${VERSION}
	DESCRIPTION "cmdr-cxx is a POSIX-compliant command-line arguments parser in C++, its part of cmdr series."
	LANGUAGES CXX)

set(VERSION_H_NAME "cmdr-version.hh")

# include(cxx-standard-def)
include(cxx-detect-compilers)
include(setup-build-env)

debug_print_top_vars()

# # TPL ## HEAD END ##########
set(PROJECT_MACRO_NAME cmdr-cxx)
set(PROJECT_MACRO_SHORT_NAME cmdr)
set(PROJECT_MACRO_MID_NAME cmdr11)
set(PROJECT_MACRO_PREFIX "CMDR")

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
