
macro(debug msg)
    message(STATUS "DEBUG ${msg}")
endmacro()

macro(debug_print_value variableName)
    debug("${variableName}=\${${variableName}}")
endmacro()

macro(debug_print_list_value listName)
    message(STATUS "- List of ${listName} -------------")
    foreach (lib ${${listName}})
        message("                         ${lib}")
    endforeach (lib)
endmacro()

macro(ensure_options_values PROJ_NAME PROJ_PREFIX)
    if (NOT DEFINED _${PROJ_NAME}_enable_assertions)
        if (DEFINED ${PROJ_PREFIX}_ENABLE_ASSERTIONS)
            SET(_${PROJ_NAME}_enable_assertions 1)
        else ()
            SET(_${PROJ_NAME}_enable_assertions 0)
        endif ()
    endif ()
    if (NOT DEFINED _${PROJ_NAME}_enable_precondition_checks)
        if (DEFINED ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS)
            SET(_${PROJ_NAME}_enable_precondition_checks 1)
        else ()
            SET(_${PROJ_NAME}_enable_precondition_checks 0)
        endif ()
    endif ()
    if (NOT DEFINED _${PROJ_NAME}_enable_thread_pool_ready_signal)
        if (DEFINED ${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL)
            SET(_${PROJ_NAME}_enable_thread_pool_ready_signal 1)
        else ()
            SET(_${PROJ_NAME}_enable_thread_pool_ready_signal 0)
        endif ()
    endif ()
    if (NOT DEFINED _${PROJ_NAME}_enable_verbose_log)
        if (DEFINED ${PROJ_PREFIX}_ENABLE_VERBOSE_LOG)
            SET(_${PROJ_NAME}_enable_verbose_log 1)
        else ()
            SET(_${PROJ_NAME}_enable_verbose_log 0)
        endif ()
    endif ()
    if (NOT DEFINED _${PROJ_NAME}_enable_thread_pool_dbgout)
        if ((DEFINED ${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT) OR ${USE_DEBUG})
            SET(_${PROJ_NAME}_enable_thread_pool_dbgout 1)
        else ()
            SET(_${PROJ_NAME}_enable_thread_pool_dbgout 0)
        endif ()
    endif ()
    if (NOT DEFINED _${PROJ_NAME}_unit_test)
        if ((DEFINED ${PROJ_PREFIX}_UNIT_TEST) OR ${USE_DEBUG})
            SET(_${PROJ_NAME}_unit_test 1)
        else ()
            SET(_${PROJ_NAME}_unit_test 0)
        endif ()
    endif ()
    message(STATUS "Config (CMAKE_BUILD_NAME): ${CMAKE_BUILD_NAME}")
endmacro()

macro(define_installable_cxx_library_project PROJ_NAME PROJ_PREFIX)
    set(_detail_header_files ${${PROJ_PREFIX}_detail_header_files})
    set(_header_files ${${PROJ_PREFIX}_header_files})
    debug_print_list_value(_header_files)
    # debug_print_value(${PROJ_NAME})

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS ON)
    #
    # Just for QT app ...
    #set(CMAKE_AUTOMOC ON)    # Qt moc, meta-object compiler
    #set(CMAKE_AUTORCC ON)    # Qt rcc, resources compiler
    #set(CMAKE_AUTOUIC ON)    # Qt uic, User-Interface compiler


    ensure_options_values(${PROJ_NAME} ${PROJ_PREFIX})
    gen_versions(${PROJ_NAME} ${PROJ_PREFIX}
            ${PROJ_NAME}-version.hh
            ${PROJ_NAME}-config.hh
            ${PROJ_NAME}-${PROJECT_VERSION}
            ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
            ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
            )


    add_library(${PROJ_NAME} INTERFACE)
    target_sources(${PROJ_NAME} INTERFACE "$<BUILD_INTERFACE:${_detail_header_files};${_header_files}>")
    target_include_directories(${PROJ_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
    target_include_directories(${PROJ_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
    #    target_compile_definitions(${PROJ_NAME} INTERFACE
    #            ${PROJ_PREFIX}_ENABLE_ASSERTIONS=${_${PROJ_NAME}_enable_assertions}
    #            ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJ_NAME}_enable_precondition_checks}
    #            ${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJ_NAME}_enable_thread_pool_ready_signal}
    #            ${PROJ_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJ_NAME}_enable_verbose_log}
    #            ${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJ_NAME}_enable_thread_pool_dbgout}
    #            #${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
    #            )
    #    set_target_properties(${PROJ_NAME} PROPERTIES
    #            ${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS=${_${PROJECT_MACRO_NAME}_enable_assertions}
    #            ${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJECT_MACRO_NAME}_enable_precondition_checks}
    #            ${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJECT_MACRO_NAME}_enable_thread_pool_ready_signal}
    #            ${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJECT_MACRO_NAME}_enable_verbose_log}
    #            ${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJECT_MACRO_NAME}_enable_thread_pool_dbgout}
    #            ${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
    #            #${PROJECT_MACRO_PREFIX}_UNIT_TEST=0
    #            #UNIT_TESTING=0
    #            )
    #target_link_libraries(fsm_cxx INTERFACE debug_assert)

    if (MSVC)
        target_compile_options(${PROJ_NAME} INTERFACE /wd4800 # truncation to bool warning
                #/D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
                )
    else ()
        target_compile_options(${PROJ_NAME} INTERFACE
                #-D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
                )
    endif ()

    # Setup package config
    include(CMakePackageConfigHelpers)
    set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/${PROJ_NAME})

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake "
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJ_NAME}-targets.cmake)
set(${PROJ_NAME}_LIBRARY ${PROJ_NAME})
set(${PROJ_NAME}_LIBRARIES ${PROJ_NAME})
")

    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
            VERSION ${VERSION}
            COMPATIBILITY SameMajorVersion
    )

    # Install target and header
    install(DIRECTORY include/${PROJ_NAME} DESTINATION include)

    install(FILES include/${PROJ_NAME}.hh DESTINATION include)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
            DESTINATION
            ${CONFIG_PACKAGE_INSTALL_DIR})

    # Only export target when using imported targets
    if (${${PROJ_PREFIX}_HAS_IMPORTED_TARGETS})

        install(TARGETS ${PROJ_NAME}
                EXPORT ${PROJ_NAME}-targets
                DESTINATION lib)

        install(EXPORT ${PROJ_NAME}-targets
                DESTINATION
                ${CONFIG_PACKAGE_INSTALL_DIR}
                )

    endif ()

    # other subdirectories
    # only add if not inside add_subdirectory()
    option(${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES "build test and example" OFF)
    if (${${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
        enable_testing()
        add_subdirectory(examples/)
        add_subdirectory(tests/)
    endif ()

    option(${PROJ_PREFIX}_BUILD_DOCS "generate documentation" OFF)
    if (${PROJ_PREFIX}_BUILD_DOCS OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/")
            find_package(Doxygen)
            if (NOT DOXYGEN_FOUND)
                set(${PROJ_PREFIX}_BUILD_DOCS OFF)
            else ()
                if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
                    set(${PROJ_PREFIX}_BUILD_DOCS ON)
                endif ()
                if ((${USE_DEBUG}) OR ($ENV{CI_RUNNING}))
                    set(${PROJ_PREFIX}_BUILD_DOCS OFF)
                endif ()
            endif ()
            if (${PROJ_PREFIX}_BUILD_DOCS)
                message(STATUS "- docs/ including ....")

                # Find all the public headers
                get_target_property(MY_PUBLIC_HEADER_DIR ${PROJECT_MACRO_NAME} INTERFACE_INCLUDE_DIRECTORIES)
                file(GLOB_RECURSE MY_PUBLIC_HEADERS ${MY_PUBLIC_HEADER_DIR}/*.hh)
                debug_print_list_value(MY_PUBLIC_HEADER_DIR)
                debug_print_value(PROJECT_SOURCE_DIR)

                #set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR} )
                set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/)
                set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
                #set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
                set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/Doxyfile)
                set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
                set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/${PROJECT_MACRO_MID_NAME}.in.doxygen)

                #Replace variables inside @@ with the current values
                configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

                file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
                add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                        DEPENDS ${MY_PUBLIC_HEADERS}
                        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                        #COMMAND
                        #  $<$<CONFIG:Release>:${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}>
                        #  $<$<NOT:$<CONFIG:Release>>:${CMAKE_COMMAND} -E "echo 'Only done in Release builds'">
                        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                        COMMENT "Generating docs ..."
                        VERBATIM)

                add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

                #    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)

                add_subdirectory(docs/)

                attach_doxygen_to(${PROJ_NAME})
                message(STATUS "- docs/ included ----")
            endif ()
        else ()
            message(WARNING "docs/ folder not exists but ${PROJ_PREFIX}_BUILD_DOCS is ON.")
        endif ()
    endif ()

endmacro()

macro(attach_doxygen_to target)
    if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
        add_dependencies(${target} docs)
    endif ()
endmacro()


function(add_cxx_17_to target)
    set_target_properties(${target}
            PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS OFF          # use -std=c++11 rather than -std=gnu++11
            )
endfunction()

function(add_cxx_20_to target)
    set_target_properties(${target}
            PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS OFF          # use -std=c++11 rather than -std=gnu++11
            )
endfunction()

