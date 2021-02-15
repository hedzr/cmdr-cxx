
#
# FOR CMDR11
#

#message(STATUS "cmdr-cxx: defined macro add_cmdr_cxx_to")
macro(add_cmdr_cxx_to target)

  find_package(cmdr11)

  set(CMDR_CXX_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmdr-cxx-stage")

  if (NOT CMDR11_FOUND)
    message(STATUS "cmdr-cxx: not found, try pulling a local one...")

    set(CMDR_CXX_TGT_NAME "third-cmdr-cxx")
    include(ExternalProject)
    ExternalProject_Add(${CMDR_CXX_TGT_NAME}
                        GIT_REPOSITORY https://github.com/hedzr/cmdr-cxx
                        GIT_TAG origin/master    # v0.3.13
                        GIT_SHALLOW 1
                        GIT_PROGRESS ON
                        # STEP_TARGETS build
                        # SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/cmdr-cxx-src"
                        # BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmdr-cxx-build"
                        CMAKE_ARGS
                        -DENABLE_TESTS=OFF
                        -DENABLE_CLI_APP=OFF
                        -DENABLE_AUTOMATE_TESTS=OFF
                        -DBUILD_DOCUMENTATION=OFF
                        --no-warn-unused-cli
                        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                        -DCMAKE_INSTALL_PREFIX:PATH=${CMDR_CXX_STAGE_DIR}
                        # -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
                        # -DCMAKE_INSTALL_PREFIX:PATH=${EXECUTABLE_OUTPUT_PATH}
                        BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting $<CONFIG> build, install_prefix: ${CMAKE_INSTALL_PREFIX}"
                        # COMMAND ${CMAKE_COMMAND} -E sudo "chmod a+w /usr/local/lib"
                        COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
                        COMMAND ${CMAKE_COMMAND} -E echo "$<CONFIG> build complete"
                        )
    #    set(CMDR11_FOUND ON)

    message(STATUS "cmdr-cxx: add ${CMDR_CXX_TGT_NAME} module to '${target}' from building dir.")
    message(STATUS "cmdr-cxx:    CI_RUNNING = $ENV{CI_RUNNING}")
    message(STATUS "cmdr-cxx: add_dependencies")
    add_dependencies(${target} ${CMDR_CXX_TGT_NAME})

  else ()

    message(STATUS "cmdr-cxx: package found at ${CMDR11_INCLUDE_DIR}, ${CMDR11_VERSION}")
    message(STATUS "cmdr-cxx: add ${CMDR_CXX_TGT_NAME} module to '${target}' from CMake Modules registry.")
    target_link_libraries(${target}
                          PRIVATE
                          ${CMDR11_LIBRARIES}
                          )

  endif ()

  #if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin" OR APPLE)
  target_include_directories(${target} PRIVATE
                             $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                             $<INSTALL_INTERFACE:include>
                             /usr/local/include
                             ${CMDR_CXX_STAGE_DIR}/include
                             # ${CMDR11_INCLUDE_DIR}
                             )
  target_link_directories(${target} PRIVATE
                          /usr/local/lib
                          ${CMDR_CXX_STAGE_DIR}/lib
                          # ${CMDR11_LIBRARY_DIR}
                          # ${CMAKE_CURRENT_BINARY_DIR}/${CMDR_CXX_TGT_NAME}-prefix/src/${CMDR_CXX_TGT_NAME}-build
                          )
  message(STATUS "cmdr-cxx: include-dir = ${CMDR_CXX_STAGE_DIR}/include")
  #endif ()

endmacro()
