project(cmdr11
        VERSION ${CMAKE_PROJECT_VERSION}
        DESCRIPTION "libcmdr11 - cmdr cxx library for C++17"
        LANGUAGES CXX)


## OPTIONS ######################################

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(_cmdr_default_assertions ON)
else ()
    set(_cmdr_default_assertions OFF)
endif ()

option(CMDR_ENABLE_ASSERTIONS "whether or not to enable internal assertions for the cmdr cxx11 library" ${_cmdr_default_assertions})
if (${CMDR_ENABLE_ASSERTIONS})
    set(_cmdr_enable_assertions 1)
else ()
    set(_cmdr_enable_assertions 0)
endif ()

option(CMDR_ENABLE_PRECONDITION_CHECKS "whether or not to enable precondition checks" ON)
if (${CMDR_ENABLE_PRECONDITION_CHECKS})
    set(_cmdr_enable_precondition_checks 1)
else ()
    set(_cmdr_enable_precondition_checks 0)
endif ()

option(CMDR_ENABLE_WRAPPER "whether or not the wrappers in cmdr-cxx.hpp are used" ON)
if (${CMDR_ENABLE_WRAPPER})
    set(_cmdr_enable_wrapper 1)
else ()
    set(_cmdr_enable_wrapper 0)
endif ()

option(CMDR_ENABLE_VERBOSE_LOG "whether or not print the matching decisions to logging output in running" OFF)
if (${CMDR_ENABLE_VERBOSE_LOG})
    set(_cmdr_enable_verbose_log 1)
else ()
    set(_cmdr_enable_verbose_log 0)
endif ()


## SETTINGS ######################################

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
#if (POLICY CMP0069)
#    cmake_policy(SET CMP0069 NEW)
#endif ()
if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()

# option(BUILD_TESTING "Build the testing tree." ON)
#
# To enable Nightly Target and More, uncomment and include CTest script from CMake bundle.
# include(CTest)


#find_package(ZLIB REQUIRED)
#find_package(fmt)
#include(deps_inc_fmt)
if (CMDR_ENABLE_VERBOSE_LOG)
    # include(deps_inc_spdlog)
    # Or for a spdlog standalone install:
    # find_package(spdlog REQUIRED)
endif ()

#debug_print_hbar()
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()

set(detail_header_files
    )
set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cmdr-cxx.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr11.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_app.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_app_inl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_arg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_chrono.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_cmd.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_cmd_inl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_cmn.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_dbg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_defs.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_if.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_imp1.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_impl.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_internals.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_ios.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_log.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_opts.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_path.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_public.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_small_string.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_string.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_terminal.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_types.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_types_check.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_utils.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_var_t.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/cmdr_var_t_inl.h
    )

## TARGETS ######################################

add_library(${PROJECT_NAME} INTERFACE)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
#cxx_set_ipo(${PROJECT_NAME})
#set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "libs/${PROJECT_NAME}")


target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)
#target_sources(${PROJECT_NAME} PRIVATE src/${PROJECT_NAME}.cc src/private.cc)
target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${detail_header_files};${header_files}>")
#set_target_properties(${PROJECT_NAME} PROPERTIES
#        COMPILE_FEATURES cxx_std_14
#        SOURCES src/${PROJECT_NAME}.cc
#        )
target_include_directories(${PROJECT_NAME}
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           #$<INSTALL_INTERFACE:include>
                           #PRIVATE
                           #${ZLIB_INCLUDE_DIRS}
                           #${spdlog_SOURCE_DIR}/include
                           )
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

target_compile_definitions(${PROJECT_NAME} INTERFACE
                           CMDR_ENABLE_ASSERTIONS=${_cmdr_enable_assertions}
                           CMDR_ENABLE_PRECONDITION_CHECKS=${_cmdr_enable_precondition_checks}
                           CMDR_ENABLE_WRAPPER=${_cmdr_enable_wrapper}
                           CMDR_ENABLE_VERBOSE_LOG=${_cmdr_enable_verbose_log})
#target_link_libraries(${PROJECT_NAME} INTERFACE debug_assert)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} PRIVATE libs::sm-lib)
#target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt-header-only)
if (CMDR_ENABLE_VERBOSE_LOG)
    #    target_link_libraries(${PROJECT_NAME} INTERFACE spdlog::spdlog)
    #target_link_libraries(${PROJECT_NAME} INTERFACE spdlog::spdlog)
    #add_dependencies(${PROJECT_NAME} spdlog)
endif ()

if (MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /wd4800) # truncation to bool warning
endif ()


set(CLI_NAME cmdr11-cli)
add_executable(${CLI_NAME} src/main.cc)
target_include_directories(${CLI_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>)
target_link_libraries(${CLI_NAME} PRIVATE libs::cmdr11)


## TESTING ######################################

if (BUILD_TESTING)
    message(STATUS trying add test with name ${PROJECT_NAME}_tests)
    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
endif ()


## INSTALLING ######################################

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CONFIG_PACKAGE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        VERSION ${CMAKE_PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

install(TARGETS ${CLI_NAME}
        EXPORT ${PROJECT_NAME}Targets
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CONFIG_PACKAGE_INSTALL_DIR})

install(DIRECTORY include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )

install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cmdr-cxx.hh
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES
        ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake
        DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

#install(TARGETS ${PROJECT_NAME}
#        # COMPONENT applications
#        # DESTINATION "bin"
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#        )


## PACKAGING ######################################

# build a CPack driven installer package
include(InstallRequiredSystemLibraries)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_SOURCE_DIR}/build
    ${PROJECT_SOURCE_DIR}/cmake-build-debug
    ${PROJECT_SOURCE_DIR}/dist
    ${PROJECT_SOURCE_DIR}/.idea
    ${PROJECT_SOURCE_DIR}/.DS_Store
    ${PROJECT_SOURCE_DIR}/.git
    ${PROJECT_SOURCE_DIR}/.gitignore
    ${PROJECT_SOURCE_DIR}/.vscode
    ${PROJECT_SOURCE_DIR}/.PIC
    ${PROJECT_SOURCE_DIR}/ref
    ${PROJECT_SOURCE_DIR}/_assets/*)
#set(CPACK_SOURCE_GENERATOR "TXZ")
#set(CPACK_SOURCE_PACKAGE_FILE_NAME ${ARCHIVE_NAME})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_DESCRIPTION "A C++17 header-only command-line parser with hierarchical config data manager")
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
set(CPACK_PACKAGE_CONTACT "Hedzr Yeh <hedzrz@gmail.com>")

if (DISTRO_NAME STREQUAL "Debian")
    message(STATUS ">>>> Found Debian <<<<")
    include(package-deb)
elseif (DISTRO_NAME STREQUAL "Ubuntu")
    message(STATUS ">>>> Found Ubuntu <<<<")
    include(package-deb)
elseif (DISTRO_NAME STREQUAL "Fedora")
    message(STATUS ">>>> Found Fedora <<<<")
    include(package-rpm)
elseif (DISTRO_NAME STREQUAL "RedHat")
    message(STATUS ">>>> Found RedHat <<<<")
    include(package-rpm)
elseif (macOS)
    message(STATUS ">>>> Found macOS/Darwin <<<<")
    include(package-dmg)
else ()
    message(STATUS ">>>> Found unknown distribution <<<<")
endif ()

#if (NOT CPack_CMake_INCLUDED)
include(CPack)
#endif ()


## DEBUG ######################################


# debug_dump_target_inc_dirs(${PROJECT_NAME})

debug_print_value(ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(CMAKE_CURRENT_SOURCE_DIR)
debug_print_value(PROJECT_SOURCE_DIR)
debug_print_value(CPACK_PACKAGE_DIRECTORY)
debug_print_value(CPACK_PROJECT_VERSION)

#debug_dump_target_inc_dirs_interface_only(${PROJECT_NAME})
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "    = '${dir}'")
endforeach ()

get_directory_property(inc_dirs INCLUDE_DIRECTORIES)
list(TRANSFORM inc_dirs PREPEND "-I ")
list(JOIN inc_dirs " " INCLUDE_STRING)
debug_print_value(inc_dirs)