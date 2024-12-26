#
# prerequisites
#

macro(add_to_cmake_module_path loc)
    if(NOT "${loc}" IN_LIST CMAKE_MODULE_PATH)
        set(CMAKE_MODULE_PATH
            ${loc} # for .version.cmake & .options.cmake
            ${CMAKE_MODULE_PATH}
        )
    endif()
endmacro(add_to_cmake_module_path loc)

macro(check_cmake_modules_dir loc name)
    if(EXISTS ${loc}/${name})
        set(CMAKE_SCRIPTS "${name}")

        add_to_cmake_module_path("${loc}")
        add_to_cmake_module_path("${loc}/${CMAKE_SCRIPTS}")
        add_to_cmake_module_path("${loc}/${CMAKE_SCRIPTS}/Modules")
    endif(EXISTS ${loc}/${name})
    if(EXISTS ${loc}/addons)
        add_to_cmake_module_path("${loc}/addons")
    endif(EXISTS ${loc}/addons)
endmacro(check_cmake_modules_dir loc)

message(STATUS "| CMAKE_MODULE_PATH             = ${CMAKE_MODULE_PATH}")

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")
check_cmake_modules_dir(${CMAKE_CURRENT_SOURCE_DIR} "cmake")

# check_cmake_modules_dir(${CMAKE_SOURCE_DIR})
message(STATUS "| CMAKE_MODULE_PATH             = ${CMAKE_MODULE_PATH}")
message(STATUS "| CMAKE_TOOLCHAIN_FILE          = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "| VCPKG_TARGET_TRIPLET          = $ENV{VCPKG_TARGET_TRIPLET}") # =x64-windows, ...
message("")

include(add-policies)

cmake_policy(SET CMP0048 NEW) # disable not-defined warning about VERSION
include(debug-cmd)

set(default_build_type "Release")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif()

debug_print_value(CMAKE_BUILD_TYPE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "" AND "${CMAKE_CONFIGURATION_TYPES}" STREQUAL "")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
endif()

if("${CMAKE_CONFIGURATION_TYPES}" STREQUAL "")
    # Set the possible values of build type for cmake-gui
    set(allowedBuildTypes Asan Debug Release RelWithDebInfo MinSizeRel)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")

    # set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "Asan")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")

    # set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)
    message(STATUS "CMAKE_CONFIGURATION_TYPES updated: ${CMAKE_CONFIGURATION_TYPES}")
    message(STATUS "CMAKE_BUILD_TYPE updated: ${CMAKE_BUILD_TYPE}")
endif()

if(("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") AND (NOT (${WIN32})))
    # In non-win32 debug build, debug_malloc is on by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." ON)
    option(USE_DEBUG "Building with DEBUG Mode" ON)
    set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
else()
    # In win32 or non-debug builds, debug_malloc is off by default
    option(USE_DEBUG_MALLOC "Building with memory leak detection capability." OFF)
    option(USE_DEBUG "Building with NON-DEBUG Mode" OFF)

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_BUILD_NAME "dbg" CACHE STRING "" FORCE)
        set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "" FORCE)
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CMAKE_BUILD_NAME "rel" CACHE STRING "release mode" FORCE)
        set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "" FORCE)
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
        set(CMAKE_BUILD_NAME "rms" CACHE STRING "min-size release mode" FORCE)
        set(CMAKE_MINSIZEREL_POSTFIX "ms" CACHE STRING "" FORCE)
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        set(CMAKE_BUILD_NAME "rwd" CACHE STRING "release mode with debug info" FORCE)
        set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Asan")
        set(CMAKE_BUILD_NAME "asan" CACHE STRING "debug mode with sanitizer" FORCE)
        set(CMAKE_ASAN_POSTFIX "" CACHE STRING "" FORCE)
    endif()
endif()

# debug_print_value(CMAKE_BUILD_TYPE)
message(STATUS ">>> DEBUG MODE: ${CMAKE_BUILD_TYPE} -> ${CMAKE_BUILD_NAME}, '${CMAKE_DEBUG_POSTFIX}' ...")
message(STATUS ">>> USE_DEBUG_MALLOC = ${USE_DEBUG_MALLOC}, USE_DEBUG = ${USE_DEBUG} ...")
mark_as_advanced(CMAKE_BUILD_NAME) # Mark variables as used so cmake doesn't complain about them
option(ENABLE_CCACHE "Use ccache for build" ON)

if(${ENABLE_CCACHE})
    find_program(CCACHE ccache)

    if(NOT "${CCACHE}" STREQUAL "CCACHE-NOTFOUND") # # if (CCACHE)
        message(STATUS ">>> ccache found and enabled")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    else()
        message(WARNING ">>> use_ccache enabled, but ccache executable not found: ${CCACHE}")
    endif()
else()
    message(STATUS ">>> ccache disabled")
endif()

# ############################## for testing
set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS)
mark_as_advanced(UNIT_TEST_TARGETS)

#
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(_DEFAULT_TESTS ${USE_DEBUG})
else()
    set(_DEFAULT_TESTS OFF)
endif()

option(ENABLE_EXAMPLES "Enable examples" OFF)
option(ENABLE_TESTS "Enable tests" ${USE_DEBUG})
option(ENABLE_AUTOMATE_TESTS "Enable automated tests at local" OFF)
option(ENABLE_UINT_TESTS "Enable unit tests / CTest" OFF)

if($ENV{CI_RUNNING})
    set(ENABLE_AUTOMATE_TESTS OFF)
    set(ENABLE_UINT_TESTS ON)
    message(STATUS "CI_RUNNING found in env, set ENABLE_AUTOMATE_TESTS to OFF, and set UnitTest/CTest ON")
endif()

if(${ENABLE_TESTS})
    enable_testing()

    # include(CTest) # note: this adds a BUILD_TESTING which defaults to ON
endif()

option(ENABLE_PPPM_WARNINGS "Enable Pre-Process Pragma Messages Warning when compiling" OFF)

# ############################## for installing
# include(GNUInstallDirs)

# set(CMAKE_VERBOSE_MAKEFILE ON)
if((${CMAKE_VERBOSE_DEBUG} OR ${USE_DEBUG}) OR ($ENV{CI_RUNNING}))
    # Enable verbose output from Makefile builds.
    # This variable is a cache entry initialized (to FALSE) by the project() command.
    # Users may enable the option in their local build tree to get more verbose
    # output from Makefile builds and show each command line as it is launched.
    set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL ON)

    # Default value for POSITION_INDEPENDENT_CODE of targets.
    # This variable is used to initialize the POSITION_INDEPENDENT_CODE property
    # on all the targets. See that target property for additional information.
    # If set, it’s value is also used by the try_compile() command.
    set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL "ON")
    message(STATUS ">>> CMAKE_VERBOSE_DEBUG ON")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ############################## include .options.cmake
message(STATUS ">>> HOME: $ENV{HOME} / $ENV{USERPROFILE} ")
message(STATUS ">>> vcpkg tests 1: env: $ENV{VCPKG_ROOT}; def: ${VCPKG_ROOT}; ")
message(STATUS ">>> CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

include(options-def) # load .options.cmake
include(pkg-mgmt) # use `load_package_manager` macro and options: USE_CONAN or USE_VCPKG
include(utils) # more tools such as print_debug_value, ...
include(dummy-project) # a dummy target for detecting ARCH, cxx compilers, versions ...
message(STATUS " ")
message(STATUS "| PROCESSOR_ARCHITEW6432        = ${PROCESSOR_ARCHITEW6432}")
message(STATUS "| PROCESSOR_ARCHITECTURE        = ${PROCESSOR_ARCHITECTURE}")
message(STATUS "| CMAKE_APPLE_SILICON_PROCESSOR = ${CMAKE_APPLE_SILICON_PROCESSOR}")
message(STATUS "| CMAKE_SYSTEM                  = ${CMAKE_SYSTEM}")
message(STATUS "| CMAKE_SYSTEM_NAME             = ${CMAKE_SYSTEM_NAME}")
message(STATUS "| CMAKE_SYSTEM_PROCESSOR        = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "| CMAKE_SYSTEM_VERSION          = ${CMAKE_SYSTEM_VERSION}")
message(STATUS "| CMAKE_HOST_SYSTEM_PROCESSOR   = ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "| CMAKE_HOST_SYSTEM_NAME        = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "| CMAKE_HOST_SYSTEM_VERSION     = ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "| CMAKE_HOST_APPLE              = ${CMAKE_HOST_APPLE}")
message(STATUS "| CMAKE_HOST_LINUX              = ${CMAKE_HOST_LINUX}")
message(STATUS "| CMAKE_HOST_UNIX               = ${CMAKE_HOST_UNIX}")
message(STATUS "| CMAKE_HOST_WIN32              = ${CMAKE_HOST_WIN32}")
message(STATUS "| CMAKE_HOST_SOLARIS            = ${CMAKE_HOST_SOLARIS}")
message(STATUS "| CMAKE_HOST_BSD                = ${CMAKE_HOST_BSD}")
message(STATUS "| CMAKE_LIBRARY_ARCHITECTURE    = ${CMAKE_LIBRARY_ARCHITECTURE}")
message(STATUS "")

message(STATUS ">>> end of prerequisites")
# message("")
