## TPL ## HEAD ##########
cmake_minimum_required(VERSION 3.9..3.19)
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules;${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS};${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH}")
include(prerequisites)
include(version-def)
include(add-policies)
include(detect-systems)
include(target-dirs)
include(utils)
include(options)

project(cmdr-cxx11
        VERSION ${VERSION}
        DESCRIPTION "cmdr-cxx11 is a POSIX-compliant command-line arguments parser in C++, its part of cmdr series."
        LANGUAGES CXX)

include(cxx-standard-def)
include(cxx-detect-compilers)
include(setup-build-env)
include(versions-gen)

prepare_cpack()

debug_print_top_vars()
debug_print_value(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
## TPL ## HEAD END ##########


#
# add more subdirectories as submodules and sub-submodules HERE
#

# add_library(study_cmake library.cxx library.h)
add_subdirectory(libs)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)






## TPL ## TAIL ##########
apply_all_unit_tests(all_tests)     # invoke CTest unittests automatically.
dump_feature_summary()              # tail print to cmake-debug/features.log