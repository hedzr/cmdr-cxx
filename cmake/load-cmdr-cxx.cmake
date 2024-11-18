#
# TO LOADING CMDR11 / CMDR-CXX EASILY:
#
#	define_cxx_executable_project(myapp
# 		PREFIX myapp
# 		LIBRARIES ${myapp_libs}
# 		SOURCES ${myapp_source_files}
# 		INCLUDE_DIRECTORIES ${myapp_INCDIR}
#	)
#	enable_version_increaser(myapp-cli myapp my MY_)
#
#	include(load-cmdr-cxx)   # load ME here.
#	add_cmdr_cxx_to(myapp)   # attach cmdr11::cmdr11
#

#

# message(STATUS "cmdr-cxx: defined macro add_cmdr_cxx_to")
macro(add_cmdr_cxx_to target)
    find_package(cmdr11)

    set(CMDR_CXX_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmdr-cxx-stage")

    option(CMAKE_VERBOSE_DEBUG "for cmdr-cxx, enable CMAKE_VERBOSE_MAKEFILE to print command line before execute them" OFF)
    option(CMDR_NO_3RDPARTY "for cmdr-cxx, disable source codes and targets alternate to yaml-cpp and others" ON)

    if(${CMDR11_FOUND})
        message(STATUS "cmdr-cxx: package FOUND at ${CMDR11_INCLUDE_DIR}, ${CMDR11_VERSION}")
        message(STATUS "cmdr-cxx: add cmdr-cxx v${CMDR11_VERSION_STRING} module to '${target}' from CMake Modules registry.")
        target_link_libraries(${target}
            # PRIVATE
            ${CMDR11_LIBRARIES}
        )
    else()
        message(STATUS "cmdr-cxx: not found, try pulling a local one...")

        set(CMDR_CXX_TGT_NAME "third-cmdr-cxx")
        include(ExternalProject)
        ExternalProject_Add(${CMDR_CXX_TGT_NAME}
            GIT_REPOSITORY https://github.com/hedzr/cmdr-cxx
            GIT_TAG origin/master # v0.3.13
            GIT_SHALLOW 1
            GIT_PROGRESS ON

            # STEP_TARGETS build
            # SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/cmdr-cxx-src"
            # BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmdr-cxx-build"
            CMAKE_ARGS
            -DCMAKE_VERBOSE_DEBUG:BOOL=${CMAKE_VERBOSE_DEBUG}
            -DCMDR_NO_3RDPARTY:BOOL=${CMDR_NO_3RDPARTY}
            -DENABLE_TESTS:BOOL=OFF
            -DENABLE_CLI_APP:BOOL=OFF
            -DENABLE_AUTOMATE_TESTS:BOOL=OFF
            -DBUILD_DOCUMENTATION:BOOL=OFF
            -DBUILD_EXAMPLES:BOOL=OFF
            --no-warn-unused-cli
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMDR_CXX_STAGE_DIR}

            # -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            # -DCMAKE_INSTALL_PREFIX:PATH=${EXECUTABLE_OUTPUT_PATH}
            BUILD_COMMAND ${CMAKE_COMMAND} -E echo "[cmdr-cxx] Starting $<CONFIG> build, install_prefix: ${CMAKE_INSTALL_PREFIX} or ${CMDR_CXX_STAGE_DIR}, BINARY_DIR: <BINARY_DIR>"

            # COMMAND ${CMAKE_COMMAND} -E sudo "chmod a+w /usr/local/lib"
            COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
            COMMAND ${CMAKE_COMMAND} -E echo "[cmdr-cxx] $<CONFIG> build complete"
        )

        message(STATUS "cmdr-cxx: add '${CMDR_CXX_TGT_NAME}' module to '${target}' from building dir.")
        message(STATUS "cmdr-cxx:    CI_RUNNING = $ENV{CI_RUNNING}")
        message(STATUS "cmdr-cxx: add_dependencies")
        add_dependencies(${target} ${CMDR_CXX_TGT_NAME})

        set(CMDR11_INCLUDE_DIR ${CMDR_CXX_STAGE_DIR}/include)
        set(CMDR11_LIB_DIR ${CMDR_CXX_STAGE_DIR}/lib)
    endif()

    target_include_directories(${target} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
        $<INSTALL_INTERFACE:include>
        /usr/local/include
        /opt/homebrew/include

        # ${CMDR_CXX_STAGE_DIR}/include
        ${CMDR11_INCLUDE_DIR}
    )
    target_link_directories(${target} PRIVATE
        /usr/local/lib
        /opt/homebrew/lib

        # ${CMDR_CXX_STAGE_DIR}/lib
        ${CMDR11_LIBRARY_DIR}

        # ${CMAKE_CURRENT_BINARY_DIR}/${CMDR_CXX_TGT_NAME}-prefix/src/${CMDR_CXX_TGT_NAME}-build
    )
    message(STATUS "cmdr-cxx: include-dir = ${CMDR11_INCLUDE_DIR}")
endmacro()
