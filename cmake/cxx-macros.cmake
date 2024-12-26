
#find_package(Threads REQUIRED)


include(lexer-tools)


macro(ensure_options_values PROJ_NAME PROJ_PREFIX)
    if(NOT DEFINED _${PROJ_NAME}_enable_assertions)
        if(DEFINED ${PROJ_PREFIX}_ENABLE_ASSERTIONS)
            if(${PROJ_PREFIX}_ENABLE_ASSERTIONS)
                SET(_${PROJ_NAME}_enable_assertions 1)
                # message("set _${PROJ_NAME}_enable_assertions to ${${PROJ_PREFIX}_ENABLE_ASSERTIONS}...")
            else()
                SET(_${PROJ_NAME}_enable_assertions 0)
            endif()
        else()
            SET(_${PROJ_NAME}_enable_assertions 0)
            # message("set _${PROJ_NAME}_enable_assertions to 0")
        endif()
    endif()

    if(NOT DEFINED _${PROJ_NAME}_enable_precondition_checks)
        if(DEFINED ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS)
            if(${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS)
                SET(_${PROJ_NAME}_enable_precondition_checks 1)
            else()
                SET(_${PROJ_NAME}_enable_precondition_checks 0)
            endif()
        else()
            SET(_${PROJ_NAME}_enable_precondition_checks 0)
        endif()
    endif()

    if(NOT DEFINED _${PROJ_NAME}_enable_thread_pool_ready_signal)
        if(DEFINED ${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL)
            if(${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL)
                SET(_${PROJ_NAME}_enable_thread_pool_ready_signal 1)
            else()
                SET(_${PROJ_NAME}_enable_thread_pool_ready_signal 0)
            endif()
        else()
            SET(_${PROJ_NAME}_enable_thread_pool_ready_signal 0)
        endif()
    endif()

    if(NOT DEFINED _${PROJ_NAME}_enable_verbose_log)
        if(DEFINED ${PROJ_PREFIX}_ENABLE_VERBOSE_LOG)
            if(${PROJ_PREFIX}_ENABLE_VERBOSE_LOG)
                SET(_${PROJ_NAME}_enable_verbose_log 1)
            else()
                SET(_${PROJ_NAME}_enable_verbose_log 0)
            endif()
        else()
            SET(_${PROJ_NAME}_enable_verbose_log 0)
        endif()
    endif()

    if(NOT DEFINED _${PROJ_NAME}_enable_thread_pool_dbgout)
        if((DEFINED ${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT) OR ${USE_DEBUG})
            if(${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT OR ${USE_DEBUG})
                SET(_${PROJ_NAME}_enable_thread_pool_dbgout 1)
            else()
                SET(_${PROJ_NAME}_enable_thread_pool_dbgout 0)
            endif()
        else()
            SET(_${PROJ_NAME}_enable_thread_pool_dbgout 0)
        endif()
    endif()

    if(NOT DEFINED _${PROJ_NAME}_unit_test)
        if((DEFINED ${PROJ_PREFIX}_UNIT_TEST) OR ${USE_DEBUG} OR ${ENABLE_UINT_TESTS})
            if((${PROJ_PREFIX}_UNIT_TEST) OR ${USE_DEBUG} OR ${ENABLE_UINT_TESTS})
                SET(_${PROJ_NAME}_unit_test 1)
            else()
                SET(_${PROJ_NAME}_unit_test 0)
            endif()
        else()
            SET(_${PROJ_NAME}_unit_test 0)
        endif()
    endif()

    set(_${PROJ_NAME}CXXDEFS "")
    #    if (${_${PROJ_NAME}_enable_assertions})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_ENABLE_ASSERTIONS=${_${PROJ_NAME}_enable_assertions})
    #    endif ()
    #    if (${_${PROJ_NAME}_enable_precondition_checks})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJ_NAME}_enable_precondition_checks})
    #    endif ()
    #    if (${_${PROJ_NAME}_enable_thread_pool_ready_signal})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJ_NAME}_enable_thread_pool_ready_signal})
    #    endif ()
    #    if (${_${PROJ_NAME}_enable_verbose_log})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJ_NAME}_enable_verbose_log})
    #    endif ()
    #    if (${_${PROJ_NAME}_enable_thread_pool_dbgout})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJ_NAME}_enable_thread_pool_dbgout})
    #    endif ()
    #    if (${_${PROJ_NAME}_unit_test})
    list(APPEND _${PROJ_NAME}CXXDEFS ${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test})
    #    endif ()
    # message(STATUS "Config (${PROJ_NAME}) (${USE_DEBUG}) (${PROJ_PREFIX}_UNIT_TEST=${${PROJ_PREFIX}_UNIT_TEST}): ${_${PROJ_NAME}CXXDEFS}")
endmacro()

function(prepend var prefix)
    set(listVar "")

    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)

    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

macro(define_cxx_executable_project name)
    set(dicep_PARAM_OPTIONS
        INSTALL # installable?
        GENERATE_CONFIG # generate config.h and version.h
        BUILD_DOCS # build docs with doxygen? 
    )
    set(dicep_PARAM_ONE_VALUE_KEYWORDS
        PREFIX # PROJ_PREFIX
        CXXSTANDARD # such as CXXSTANDARD 17
        VERSION # default is ${PROJECT_VERSION}
    )
    set(dicep_PARAM_MULTI_VALUE_KEYWORDS
        CXXFLAGS # cxx compiler options
        CXXDEFINITIONS
        HEADERS
        DETAILED_HEADERS
        SOURCES
        INCLUDE_DIRECTORIES
        LIBRARIES
        FLEX
        BISON
    )

    cmake_parse_arguments(
        dicep_ARG
        "${dicep_PARAM_OPTIONS}"
        "${dicep_PARAM_ONE_VALUE_KEYWORDS}"
        "${dicep_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )

    set(dicep_usage "define_cxx_executable_project(<Name>
     [INSTALL] [GENERATE_CONFIG] [BUILD_DOCS] 
     [PREFIX <c-macro-prefix-name>]
     [CXXSTANDARD 17/20/11]
     [CXXFLAGS -Wall [...]]
	 [CXXDEFINITIONS A=1 B C=TRUE [...]]
     [VERSION \"\${PROJECT_VERSION}\"]
     [HEADERS a.hh b.hh ...]
     [DETAILED_HEADERS detail/a.hh...]
     [SOURCES a.cc b.cc ...]
     [INCLUDE_DIRECTORIES ../include ./include ...]
     [LIBRARIES rt thread ...]

     [FLEX <my-flex-target> <scanner.l> [options...]]
     [BISON <my-bison-target> <parser.l> [options...]]
     )
     
     prefix-name should be UPPERCASE since it's primary used as part of C/C++ Macro Name.
     
     Unparsed Params Are:
     ${dicep_ARG_UNPARSED_ARGUMENTS}
	"    )

    if(NOT "${dicep_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(SEND_ERROR ${dicep_usage})
    else()
        # # set(_src_list "${dicep_MULTI_VALUE_KEYWORDS}")
        # set(_src_list)
        # foreach (f ${dicep_ARG_UNPARSED_ARGUMENTS})
        #     list(APPEND _src_list ${f})
        # endforeach ()

        if("${dicep_ARG_PREFIX}" STREQUAL "")
            string(SUBSTRING "${name}" 0 3 dicep_ARG_PREFIX)
        endif()

        #        set(_dicep_type)
        #        if (${dicep_ARG_INTERFACE})
        #            set(_dicep_type INTERFACE)
        #        endif ()
        #        if (${dicep_ARG_SHARED})
        #            set(_dicep_type SHARED)
        #        endif ()
        #        if (${dicep_ARG_STATIC})
        #            set(_dicep_type STATIC)
        #        endif ()
        #        if (${dicep_ARG_MODULE})
        #            set(_dicep_type MODULE)
        #        endif ()

        set(PROJ_NAME ${name})
        set(PROJ_PREFIX ${dicep_ARG_PREFIX})

        if(NOT "${PROJECT_MACRO_PREFIX}" STREQUAL "")
            set(_macro_name_prefix "${PROJECT_MACRO_PREFIX}")
        else()
            set(_macro_name_prefix "${PROJ_PREFIX}")
        endif()
        ensure_options_values(${PROJ_NAME} ${_macro_name_prefix})

        debug("--------- Executable ---------> ${PROJ_NAME} (prefix: ${PROJ_PREFIX}) with type: \"${_dicep_type}\" | SRC: ${dicep_ARG_SOURCES}")

        if("${dicep_ARG_VERSION}" STREQUAL "")
            set(dicep_ARG_VERSION "${PROJECT_VERSION}")
        endif()
        if("${dicep_ARG_CXXSTANDARD}" STREQUAL "")
            set(dicep_ARG_CXXSTANDARD "17")
        endif()

        if(dicep_ARG_GENERATE_CONFIG)
            gen_versions(${PROJ_NAME} ${PROJ_PREFIX}
                ${PROJ_NAME}-version.hh
                ${PROJ_NAME}-config.hh
                ${PROJ_NAME}-${dicep_ARG_VERSION}
                ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
                ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
            )
        endif()

        list(APPEND dicep_ARG_INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_SOURCE_DIR}/include")

        add_executable(${PROJ_NAME} ${dicep_ARG_SOURCES})

        target_sources(${PROJ_NAME} PRIVATE
            "$<BUILD_INTERFACE:${dicep_ARG_DETAILED_HEADERS};${dicep_ARG_HEADERS}>")

        set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)

        add_cxx_standard_to(${PROJ_NAME} ${dicep_ARG_CXXSTANDARD})
        # target_compile_features(${PROJ_NAME} PRIVATE "cxx_std_${dicep_ARG_CXXSTANDARD}")

        target_compile_definitions(${PROJ_NAME} PRIVATE
            ${_${PROJ_NAME}CXXDEFS}
            ${dicep_ARG_CXXDEFINITIONS}
        )

        if(MSVC)
            target_compile_options(${PROJ_NAME} PRIVATE
                /W4 /WX /utf-8 /wd4800 # truncation to bool warning
                /Zc:__cplusplus
                #-pedantic -Wall -Wshadow -Werror
                #-Wdeprecated-declarations
                ${dicep_ARG_CXXFLAGS}
            )
        else()
            target_compile_options(${PROJ_NAME} PRIVATE
                -pedantic -Wall -Wextra -Wshadow -Werror -pthread
                -Wdeprecated-declarations
                # -fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
                ${dicep_ARG_CXXFLAGS}
            )
        endif()

        target_include_directories(${PROJ_NAME}
            PUBLIC
            ${dicep_ARG_INCLUDE_DIRECTORIES}
            PRIVATE
            # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        )
        target_include_directories(${PROJ_NAME}
            SYSTEM PRIVATE
            $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
        )

        target_link_libraries(${PROJ_NAME} ${dicep_ARG_LIBRARIES})

        # message(STATUS "----------- s2 ------------")
        set(_defs ${${PROJ_PREFIX}_cxx_defs})

        if(MSVC)
            list(TRANSFORM _defs PREPEND /D)
            target_compile_options(${PROJ_NAME} INTERFACE /wd4800 # truncation to bool warning
                /Zc:__cplusplus
                ${_defs}

                # /D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
            )
        else()
            list(TRANSFORM _defs PREPEND -D)
            target_compile_options(${PROJ_NAME} INTERFACE
                ${_defs}

                # -D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
            )
        endif()


        if(NOT dicep_ARG_FLEX AND NOT dicep_ARG_BISON)
        else()
            debug_print_value(dicep_ARG_FLEX)
            debug_print_value(dicep_ARG_BISON)
            add_lexer_targets_to(${PROJ_NAME}
                FLEX ${dicep_ARG_FLEX}
                BISON ${dicep_ARG_BISON}
                ${dicep_ARG_UNPARSED_ARGUMENTS}
            )
        endif()

    endif()

    #    set(_source_files ${${PROJ_PREFIX}_source_files})
    #    set(_header_files ${${PROJ_PREFIX}_header_files})
    #    set(_detail_header_files ${${PROJ_PREFIX}_detail_header_files})
    #    # message(STATUS "----------- s --- 1")
    #    debug_print_list_value(_header_files)
    #    # message(STATUS "----------- s --- 2")
    #    debug_print_list_value(_source_files)
    #
    #    # debug_print_value(${PROJ_NAME})
    #    set(CMAKE_CXX_STANDARD 17)
    #    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    #    set(CMAKE_CXX_EXTENSIONS ON)
    #
    #    #
    #    # Just for QT app ...
    #    # set(CMAKE_AUTOMOC ON)    # Qt moc, meta-object compiler
    #    # set(CMAKE_AUTORCC ON)    # Qt rcc, resources compiler
    #    # set(CMAKE_AUTOUIC ON)    # Qt uic, User-Interface compiler
    #    ensure_options_values(${PROJ_NAME} ${PROJ_PREFIX})
    #    gen_versions(${PROJ_NAME} ${PROJ_PREFIX}
    #            ${PROJ_NAME}-version.hh
    #            ${PROJ_NAME}-config.hh
    #            ${PROJ_NAME}-${PROJECT_VERSION}
    #            ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
    #            ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
    #            )
    #
    #    # message(STATUS "----------- s1, executable: ${PROJ_NAME} ------------")
    #    add_executable(${PROJ_NAME} ${_source_files})
    #    target_sources(${PROJ_NAME} PRIVATE "$<BUILD_INTERFACE:${_detail_header_files};${_header_files}>")
    #    target_include_directories(${PROJ_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
    #    target_include_directories(${PROJ_NAME} SYSTEM PRIVATE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
    #
    #    # message(STATUS "----------- s2 ------------")
    #    set(_defs ${${PROJ_PREFIX}_cxx_defs})
    #
    #    if (MSVC)
    #        list(TRANSFORM _defs PREPEND /D)
    #        target_compile_options(${PROJ_NAME} INTERFACE /wd4800 # truncation to bool warning
    #                ${_defs}
    #
    #                # /D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
    #                )
    #    else ()
    #        list(TRANSFORM _defs PREPEND -D)
    #        target_compile_options(${PROJ_NAME} INTERFACE
    #                ${_defs}
    #
    #                # -D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
    #                )
    #    endif ()
endmacro()

# PREREQUISITES:
#    CMAKE_SCRIPTS MUST BE defined as "cmake" or your cmake scripts folder
#      we assume the *.cmake put at "${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}"
#      includes version.h.in and config-base.h.in
#
macro(define_cxx_library_project name)
    set(diclp_PARAM_OPTIONS
        INTERFACE # interface or public
        STATIC
        SHARED
        MODULE
        # PUBLIC             # interface or public
        INSTALL # installable?
        PACK # CPack?
        GENERATE_CONFIG # generate config.h and version.h
        BUILD_DOCS # build docs with doxygen? 
    )
    set(diclp_PARAM_ONE_VALUE_KEYWORDS
        PREFIX # PROJ_PREFIX
        CXXSTANDARD # such as CXXSTANDARD 17
        # OPTIONS            # cxx compiler options
        VERSION # default is ${PROJECT_VERSION}
        INSTALL_INC_DIR # headers in <INSTALL_INC_DIR>/<Name> will be installed
    )
    set(diclp_PARAM_MULTI_VALUE_KEYWORDS
        CXXFLAGS # cxx compiler options
        CXXDEFINITIONS
        HEADERS # public headers
        DETAILED_HEADERS # more headers
        SOURCES #  
        INCLUDE_DIRECTORIES #
        LIBRARIES #
        FLEX
        BISON
    )

    cmake_parse_arguments(
        diclp_ARG
        "${diclp_PARAM_OPTIONS}"
        "${diclp_PARAM_ONE_VALUE_KEYWORDS}"
        "${diclp_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )

    set(diclp_usage "define_cxx_library_project(<Name>
      [INTERFACE | STATIC | SHARED | MODULE]
      
      [INSTALL] [PACK] [GENERATE_CONFIG] [BUILD_DOCS] 
      [PREFIX <c-macro-prefix-name>]
      [CXXSTANDARD 17/20/11]
      [CXXFLAGS -Wall [...]]    # cxx options, such as \"-Wdeprecated-declarations -Wno-unused-function\"
	  [CXXDEFINITIONS A=1 B C=TRUE [...]]
      [VERSION \"\${PROJECT_VERSION}\"]
      [HEADERS a.hh b.hh ...]
      [DETAILED_HEADERS detail/a.hh...]
      [SOURCES a.cc b.cc ...]
      [INCLUDE_DIRECTORIES ../include ./include ...]
      [INSTALL_INC_DIR include]  # headers in <INSTALL_INC_DIR>/<Name> will be installed
 
      [FLEX <my-flex-target> <scanner.l> [options...]]
      [BISON <my-bison-target> <parser.l> [options...]]
      )
      
      prefix-name should be UPPERCASE since it's primary used as part of C/C++ Macro Name.
      
      Unparsed Params Are:
      ${diclp_ARG_UNPARSED_ARGUMENTS}
	"    )

    if(NOT "${diclp_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(SEND_ERROR ${diclp_usage})
    else()
        # # set(_src_list "${dicep_MULTI_VALUE_KEYWORDS}")
        # set(_src_list)
        # foreach (f ${dicep_ARG_UNPARSED_ARGUMENTS})
        #     list(APPEND _src_list ${f})
        # endforeach ()

        if("${diclp_ARG_PREFIX}" STREQUAL "")
            string(SUBSTRING "${name}" 0 3 diclp_ARG_PREFIX)
        endif()

        set(_diclp_type)
        if(${diclp_ARG_INTERFACE})
            set(_diclp_type INTERFACE)
        endif()
        if(${diclp_ARG_SHARED})
            set(_diclp_type SHARED)
        endif()
        if(${diclp_ARG_STATIC})
            set(_diclp_type STATIC)
        endif()
        if(${diclp_ARG_MODULE})
            set(_diclp_type MODULE)
        endif()

        set(PROJ_NAME ${name})
        set(PROJ_PREFIX ${diclp_ARG_PREFIX})


        if(NOT "${PROJECT_MACRO_PREFIX}" STREQUAL "")
            set(_macro_name_prefix "${PROJECT_MACRO_PREFIX}")
        else()
            set(_macro_name_prefix "${PROJ_PREFIX}")
        endif()
        ensure_options_values(${PROJ_NAME} ${_macro_name_prefix})

        debug("--------- Library ---------> ${PROJ_NAME} (prefix: ${PROJ_PREFIX}) with type: \"${_diclp_type}\" | SRC: ${diclp_ARG_SOURCES}")
        # debug_print_value(PROJ_PREFIX)
        # debug_print_value(diclp_ARG_INTERFACE)
        # debug_print_value(diclp_ARG_INSTALL)
        # debug_print_value(diclp_ARG_BUILD_DOCS)
        # debug_print_value(diclp_ARG_GENERATE_CONFIG)
        # debug_print_value(diclp_ARG_CXXFLAGS)
        # debug_print_list_value(diclp_ARG_SOURCES)

        # set(CMAKE_CXX_STANDARD 17)
        # set(CMAKE_CXX_STANDARD_REQUIRED ON)
        # set(CMAKE_CXX_EXTENSIONS ON)

        #
        # Just for QT app ...
        # set(CMAKE_AUTOMOC ON)    # Qt moc, meta-object compiler
        # set(CMAKE_AUTORCC ON)    # Qt rcc, resources compiler
        # set(CMAKE_AUTOUIC ON)    # Qt uic, User-Interface compiler

        if("${diclp_ARG_VERSION}" STREQUAL "")
            set(diclp_ARG_VERSION "${PROJECT_VERSION}")
        endif()
        if("${diclp_ARG_CXXSTANDARD}" STREQUAL "")
            set(diclp_ARG_CXXSTANDARD "17")
        endif()

        if(diclp_ARG_GENERATE_CONFIG)
            gen_versions(${PROJ_NAME} ${PROJ_PREFIX}
                ${PROJ_NAME}-version.hh
                ${PROJ_NAME}-config.hh
                ${PROJ_NAME}-${diclp_ARG_VERSION}
                ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
                ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
            )
        endif()

        list(APPEND diclp_ARG_INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_SOURCE_DIR}/include")
        if(MSVC)
            set(_diclp_opts
                -D_CRT_SECURE_NO_WARNINGS # using getenv() ...
                -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING # using std::iterator in cxx20
            )
        else()
            set(_diclp_opts
                -pedantic -Wall -Wextra -Wshadow -Werror -pthread
                -Wdeprecated-declarations

                #-fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
                #-pedantic -Wall -Wextra -Werror=return-type -Wshadow=local -Wempty-body -fdiagnostics-color
                #-D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
            )
        endif()
        list(APPEND _diclp_opts "${diclp_ARG_CXXFLAGS}")

        debug_print_value(PROJ_NAME)
        debug_print_value(PROJ_PREFIX)
        add_library(${PROJ_NAME} ${_diclp_type})
        add_library(libs::${PROJ_NAME} ALIAS ${PROJ_NAME})
        add_library(${PROJ_NAME}::${PROJ_NAME} ALIAS ${PROJ_NAME})

        set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)

        debug_print_list_value(diclp_ARG_INCLUDE_DIRECTORIES)
        # debug_print_value(_diclp_type)
        if(NOT "${_diclp_type}" STREQUAL "INTERFACE")
            target_compile_features(${PROJ_NAME} PRIVATE "cxx_std_${diclp_ARG_CXXSTANDARD}")

            target_compile_definitions(${PROJ_NAME} PRIVATE
                ${_${PROJ_NAME}CXXDEFS}
                ${dicep_ARG_CXXDEFINITIONS}
            )

            target_compile_options(${PROJ_NAME} PRIVATE
                # -fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
                "${_diclp_opts}"
            )

            target_include_directories(${PROJ_NAME}
                PUBLIC
                ${diclp_ARG_INCLUDE_DIRECTORIES}
                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                $<INSTALL_INTERFACE:$<CMAKE_CURRENT_BINARY_DIR>>
                PRIVATE
                # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                # SYSTEM PRIVATE
                $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
            )

            target_sources(${PROJ_NAME} PRIVATE
                "$<BUILD_INTERFACE:${diclp_ARG_SOURCES}>"
            )
        else()
            target_compile_features(${PROJ_NAME} INTERFACE "cxx_std_${diclp_ARG_CXXSTANDARD}")

            target_compile_definitions(${PROJ_NAME} INTERFACE
                ${_${PROJ_NAME}CXXDEFS}
                ${dicep_ARG_CXXDEFINITIONS}
            )

            target_compile_options(${PROJ_NAME} INTERFACE
                # -fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
                "${_diclp_opts}"
            )

            target_include_directories(${PROJ_NAME}
                INTERFACE
                $<BUILD_INTERFACE:${diclp_ARG_INCLUDE_DIRECTORIES}>
                #PRIVATE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                #$<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                #$<INSTALL_INTERFACE:$<CMAKE_CURRENT_BINARY_DIR>>
                #SYSTEM INTERFACE
                $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

            target_sources(${PROJ_NAME} INTERFACE
                "$<BUILD_INTERFACE:${diclp_ARG_HEADERS};${diclp_ARG_DETAILED_HEADERS}>")
        endif()


        set(_lib_inc_dir include)
        if(NOT "${diclp_ARG_INSTALL_INC_DIR}" STREQUAL "")
            set(_lib_inc_dir "${diclp_ARG_INSTALL_INC_DIR}")
        elseif(${diclp_ARG_INCLUDE_DIRECTORIES})
            list(GET diclp_ARG_INCLUDE_DIRECTORIES 0 _lib_inc_dir)
        endif()
        get_filename_component(_lib_inc_dir ${_lib_inc_dir} REALPATH)

        # Find all the public headers
        get_target_property(${PROJ_NAME}_PUBLIC_HEADER_DIR ${PROJ_NAME} INTERFACE_INCLUDE_DIRECTORIES)
        debug_print_value(${PROJ_NAME}_PUBLIC_HEADER_DIR)
        if("${${PROJ_NAME}_PUBLIC_HEADER_DIR}" STREQUAL "")
            set(${PROJ_NAME}_PUBLIC_HEADER_DIR "${_lib_inc_dir}")
            debug_print_value(${PROJ_NAME}_PUBLIC_HEADER_DIR)
        endif()


        # target_compile_definitions(${PROJ_NAME} INTERFACE
        # ${PROJ_PREFIX}_ENABLE_ASSERTIONS=${_${PROJ_NAME}_enable_assertions}
        # ${PROJ_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJ_NAME}_enable_precondition_checks}
        # ${PROJ_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJ_NAME}_enable_thread_pool_ready_signal}
        # ${PROJ_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJ_NAME}_enable_verbose_log}
        # ${PROJ_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJ_NAME}_enable_thread_pool_dbgout}
        # #${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
        # )
        # set_target_properties(${PROJ_NAME} PROPERTIES
        # ${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS=${_${PROJECT_MACRO_NAME}_enable_assertions}
        # ${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS=${_${PROJECT_MACRO_NAME}_enable_precondition_checks}
        # ${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${PROJECT_MACRO_NAME}_enable_thread_pool_ready_signal}
        # ${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG=${_${PROJECT_MACRO_NAME}_enable_verbose_log}
        # ${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT=${_${PROJECT_MACRO_NAME}_enable_thread_pool_dbgout}
        # ${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
        # #${PROJECT_MACRO_PREFIX}_UNIT_TEST=0
        # #UNIT_TESTING=0
        # )
        # target_link_libraries(proj_cxx INTERFACE debug_assert)
        if(MSVC)
            target_compile_options(${PROJ_NAME} INTERFACE /wd4800 # truncation to bool warning
                /Zc:__cplusplus
                # /D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
            )
        else(MSVC)
            if(ENABLE_PPPM_WARNINGS)
                target_compile_options(${PROJ_NAME} INTERFACE

                    # -D${PROJ_PREFIX}_UNIT_TEST=${_${PROJ_NAME}_unit_test}
                )
            else()
                if(GCC)
                    target_compile_options(${PROJ_NAME} INTERFACE
                        -Wno-unknown-pragmas # disable #pragma message() warnings in gcc
                        -ftrack-macro-expansion=0 and -fno-diagnostics-show-caret # https://stackoverflow.com/questions/30255294/how-to-hide-extra-output-from-pragma-message
                    )
                else()
                    target_compile_options(${PROJ_NAME} INTERFACE
                        -Wno-unknown-pragmas # disable #pragma message() warnings in gcc
                    )
                endif()
            endif()
        endif(MSVC)


        if(NOT diclp_ARG_FLEX AND NOT diclp_ARG_BISON)
        else()
            debug_print_value(diclp_ARG_FLEX)
            debug_print_value(diclp_ARG_BISON)
            add_lexer_targets_to(${PROJ_NAME}
                FLEX ${diclp_ARG_FLEX}
                BISON ${diclp_ARG_BISON}
                ${diclp_ARG_UNPARSED_ARGUMENTS}
            )
        endif()


        message(STATUS "[${PROJ_NAME}] check and define install section.")

        if(diclp_ARG_INSTALL)
            # Setup package config
            include(CMakePackageConfigHelpers)

            set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/${PROJ_NAME})

            file(WRITE
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake "
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJ_NAME}-targets.cmake)
set(${PROJ_NAME}_LIBRARY ${PROJ_NAME})
set(${PROJ_NAME}_LIBRARIES ${PROJ_NAME})
"            )
            message(STATUS "[${PROJ_NAME}] file written: ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake")
            write_basic_package_version_file(
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
                VERSION ${diclp_ARG_VERSION}
                COMPATIBILITY SameMajorVersion
            )
            message(STATUS "[${PROJ_NAME}] file written: ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake")

            set(_lib_inc_prefix "${_lib_inc_dir}/${PROJ_NAME}")

            # Install target and header
            install(DIRECTORY ${_lib_inc_prefix}
                DESTINATION include
                FILES_MATCHING PATTERN "${PROJ_PREFIX}*.hh")

            if (EXISTS ${_lib_inc_prefix}.hh)
                install(FILES ${_lib_inc_prefix}.hh DESTINATION include)
                message(STATUS "[${PROJ_NAME}] install header file: ${_lib_inc_prefix}.hh")
            elseif (EXISTS ${_lib_inc_prefix}-cxx.hh)
                install(FILES ${_lib_inc_prefix}-cxx.hh DESTINATION include)
                message(STATUS "[${PROJ_NAME}] install header file: ${_lib_inc_prefix}-cxx.hh")
            endif ()

            install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}-config-version.cmake
                DESTINATION
                ${CONFIG_PACKAGE_INSTALL_DIR})

            # Only export target when using imported targets
            if(${${PROJ_PREFIX}_HAS_IMPORTED_TARGETS})
                install(TARGETS ${PROJ_NAME}
                    EXPORT ${PROJ_NAME}-targets
                    DESTINATION lib)

                install(EXPORT ${PROJ_NAME}-targets
                    NAMESPACE libs::
                    DESTINATION
                    ${CONFIG_PACKAGE_INSTALL_DIR}
                )
            endif()

        endif(diclp_ARG_INSTALL) # INSTALL
        option(${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES "build tests and examples" OFF)

        if(${${PROJ_PREFIX}_BUILD_TESTS_EXAMPLES} OR (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL CMAKE_SOURCE_DIR))
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/")
                enable_testing()
                add_subdirectory(examples/)
            endif()
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/")
                enable_testing()
                add_subdirectory(tests/)
            endif()
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/")
                enable_testing()
                add_subdirectory(test/)
            endif()
        endif()


        option(${PROJ_PREFIX}_PACKAGING "enabled CPack and packaging" OFF)
        if(${diclp_ARG_PACK} OR ${${PROJ_PREFIX}_PACKAGING})
            message(STATUS "[${PROJ_NAME}] CPack enabled.")
            # set(CPACK_PROJECT_NAME ${PROJ_NAME})
            enable_cpack(${PROJ_NAME})
        endif()


        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/")
            set(diclp_ARG_BUILD_DOCS OFF)
        endif()
        option(${PROJ_PREFIX}_BUILD_DOCS "build documentations" ${diclp_ARG_BUILD_DOCS})
        debug_print_value(${PROJ_PREFIX}_BUILD_DOCS)

        if(${PROJ_PREFIX}_BUILD_DOCS AND (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL CMAKE_SOURCE_DIR))
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/")
                find_package(Doxygen)

                if(NOT DOXYGEN_FOUND)
                    set(${PROJ_PREFIX}_BUILD_DOCS OFF)
                else()
                    if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
                        set(${PROJ_PREFIX}_BUILD_DOCS ON)
                    endif()

                    if((${USE_DEBUG}) OR ($ENV{CI_RUNNING}))
                        set(${PROJ_PREFIX}_BUILD_DOCS OFF)
                    endif()
                endif()

                if(${PROJ_PREFIX}_BUILD_DOCS)
                    message(STATUS "- docs/ including | doxygen ....")

                    # Find all the public headers
                    set(MY_PUBLIC_HEADER_DIR "${_lib_inc_dir}")
                    debug_print_value(MY_PUBLIC_HEADER_DIR)

                    file(GLOB_RECURSE MY_PUBLIC_HEADERS ${MY_PUBLIC_HEADER_DIR}/*.hh)
                    debug_print_list_value(MY_PUBLIC_HEADER_DIR)
                    debug_print_value(PROJECT_SOURCE_DIR)

                    # set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR} )
                    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/)
                    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)

                    # set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
                    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/Doxyfile)
                    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
                    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/${PROJECT_MACRO_MID_NAME}.in.doxygen)

                    # Replace variables inside @@ with the current values
                    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

                    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) # Doxygen won't create this for us
                    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                        DEPENDS ${MY_PUBLIC_HEADERS}
                        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}

                        # COMMAND
                        # $<$<CONFIG:Release>:${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}>
                        # $<$<NOT:$<CONFIG:Release>>:${CMAKE_COMMAND} -E "echo 'Only done in Release builds'">
                        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                        COMMENT "Generating docs ..."
                        VERBATIM)

                    add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

                    # install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
                    add_subdirectory(docs/)

                    attach_doxygen_to(${PROJ_NAME})
                    message(STATUS "- docs/ included | doxygen ----")
                endif()
            else()
                message(WARNING "docs/ folder not exists but ${PROJ_PREFIX}_BUILD_DOCS is ON.")
            endif()
        endif()
    endif()
endmacro()


macro(attach_doxygen_to target)
    if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
        add_dependencies(${target} docs)
    endif()
endmacro()


function(enable_cxx_standard cxxstandard)
    set(CMAKE_CXX_STANDARD ${cxxstandard})
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS ON)
endfunction(enable_cxx_standard cxxstandard)

function(add_cxx_standard_to target cxxstandard)
    set_target_properties(${target}
        PROPERTIES
        CXX_STANDARD ${cxxstandard}
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS OFF # use -std=c++11 rather than -std=gnu++11
    )
endfunction()

function(add_cxx_20_to target)
    set_target_properties(${target}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS OFF # use -std=c++11 rather than -std=gnu++11
    )
endfunction()

macro(enable_version_increaser target PROJECT_MACRO_NAME PROJECT_MACRO_SHORT_NAME PROJECT_MACRO_PREFIX)
    # cannot work on a INTERFACE library target
    find_program(PYTHON_OK "python3")
    IF(PYTHON_OK)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND python3 ${CMAKE_SOURCE_DIR}/cmake/versions-extract.py -n "${PROJECT_MACRO_NAME}" -s "${PROJECT_MACRO_SHORT_NAME}" -m "${PROJECT_MACRO_PREFIX}" -b "${CMAKE_BINARY_DIR}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "extracting version code and increase build-number part..."
            VERBATIM
        )
    ELSE()
        message(STATUS "[NOTE] version number autoincreasing was ignored since python3 is not exists.")
    ENDIF()
endmacro(enable_version_increaser)


macro(enable_cpack target)
    get_property(AUTHOR GLOBAL PROPERTY PROJECT_MAINTAINER)
    get_property(_cpack_inc GLOBAL PROPERTY CPACK_INCLUDED)
    # message(STATUS "    [enable_cpack]: AUTHOR = ${AUTHOR}")
    # message(STATUS "    [enable_cpack]: _cpack_inc = ${_cpack_inc}")
    if(NOT DEFINED ${_cpack_inc})
        set(_cpack_inc OFF)
        # message(STATUS "    [enable_cpack] set: _cpack_inc = ${_cpack_inc}")
    endif()
    if(NOT ${_cpack_inc})
        message(STATUS "    [enable_cpack]: AUTHOR = ${AUTHOR}")
        set_property(GLOBAL PROPERTY CPACK_INCLUDED ON)
        set(CPACK_PACKAGE_VENDOR "${AUTHOR}")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${AUTHOR}")
        set(CPACK_PACKAGE_HOMEPAGE "${PROJ_HOMEPAGE}")

        if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "amd64.*|x86_64.*|AMD64.*")
            set(CPU_ARCH "64" CACHE STRING "ARCH x86_64" FORCE)
            #set(CPU_ARCH_NAME "x86_64" CACHE STRING "ARCH x86_64" FORCE)
            set(CPU_ARCH_NAME "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "ARCH x86_64" FORCE)
        else()
            set(CPU_ARCH "86" CACHE STRING "ARCH x86" FORCE)
            set(CPU_ARCH_NAME "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "ARCH x86_64" FORCE)
        endif()

        if(APPLE)
            set(CPACK_GENERATOR "DragNDrop") # DMG
            set(CPACK_TARGET_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CPU_ARCH_NAME}.deb")
            message(STATUS "    [enable_cpack]: CPACK_FILE_NAME = ${CPACK_TARGET_FILE_NAME}")
        elseif(Linux)
            set(CPACK_GENERATOR "DEB;RPM;TXZ")

            set(CPACK_DEBIAN_PACKAGE_NAME "lib${CPACK_PACKAGE_NAME}-dev")
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
            set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
            set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE})
            # set(CPACK_TARGET_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CPU_ARCH_NAME}.deb")
            message(STATUS "    [enable_cpack]: CPACK_DEB_FILE_NAME = ${CPACK_DEB_FILE_NAME}")

            set(CPACK_RPM_PACKAGE_NAME "lib${CPACK_PACKAGE_NAME}-devel")
            set(CPACK_RPM_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
            set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
            set(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION})
            set(CPACK_RPM_PACKAGE_AUTOPROV YES)
            set(CPACK_RPM_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE})
            # set(CPACK_TARGET_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-1.${CMAKE_SYSTEM_PROCESSOR}.rpm")
            message(STATUS "    [enable_cpack]: CPACK_RPM_FILE_NAME = ${CPACK_RPM_FILE_NAME}")
        elseif(Windows)
            set(CPACK_GENERATOR "ZIP;SOURCE_ZIP")
        endif()
        set(CPACK_THREADS 0)
        # set(CPACK_PACKAGE_NAME "${PRJ_OSN}")
        # set(CPACK_PACKAGE_RELEASE 1)
        # set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
        # set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

        set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/packages")

        # configure_file("${CMAKE_CURRENT_SOURCE_DIR}/my_project.spec.in" "${CMAKE_CURRENT_BINARY_DIR}/my_project.spec" @ONLY IMMEDIATE)
        # set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/my_project.spec")

        include(CPack)
    endif()
endmacro()


# https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
# https://github.com/google/sanitizers
#
# https://www.jetbrains.com/help/clion/google-sanitizers.html#Configuration
#
macro(enable_sanitizer_for_multi_config)
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    if(isMultiConfig)
        if(NOT "Asan" IN_LIST CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES Asan)
            message(STATUS "[ASAN] appended Asan into CMAKE_CONFIGURATION_TYPES")
        endif()
    else()
        set(allowedBuildTypes Asan Debug Release RelWithDebInfo MinSizeRel)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")
        message(STATUS "[ASAN] set CMAKE_CONFIGURATION_TYPES for Asan support")

        if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
            message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
        endif()
    endif()

    message(STATUS "[ASAN] setting up CMAKE_CXX_FLAGS_ASAN")

    if(macOS) # after included detect-systems.cmake
        if(USING_CLANG_APPLE)
            if(CMAKE_BUILD_TYPE STREQUAL "Asan")
                message(WARNING "[ASAN] As known issue, AppleClang cannot support Address/Leak Sanitizers. Use brew install llvm and enable leak checking with llvm.")
            endif()
        else()
            message(STATUS "[ASAN] using macOS specials")
            set(CMAKE_C_FLAGS_ASAN
                "${CMAKE_C_FLAGS_DEBUG} -DASAN_ENABLED=1 -fsanitize=address -fno-omit-frame-pointer -g -O1" CACHE STRING
                "Flags used by the C compiler for Asan build type or configuration." FORCE)

            set(CMAKE_CXX_FLAGS_ASAN
                "${CMAKE_CXX_FLAGS_DEBUG} -DASAN_ENABLED=1 -fsanitize=address -fno-omit-frame-pointer -g -O1" CACHE STRING
                "Flags used by the C++ compiler for Asan build type or configuration." FORCE)

            set(CMAKE_EXE_LINKER_FLAGS_ASAN
                "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
                "Linker flags to be used to create executables for Asan build type." FORCE)

            set(CMAKE_SHARED_LINKER_FLAGS_ASAN
                "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
                "Linker lags to be used to create shared libraries for Asan build type." FORCE)
        endif()
    else()
        set(CMAKE_C_FLAGS_ASAN
            "${CMAKE_C_FLAGS_DEBUG} -DASAN_ENABLED=1 -fsanitize=address -fno-omit-frame-pointer -g -O1" CACHE STRING
            "Flags used by the C compiler for Asan build type or configuration." FORCE)

        set(CMAKE_CXX_FLAGS_ASAN
            "${CMAKE_CXX_FLAGS_DEBUG} -DASAN_ENABLED=1 -fsanitize=address -fno-omit-frame-pointer -g -O1" CACHE STRING
            "Flags used by the C++ compiler for Asan build type or configuration." FORCE)

        set(CMAKE_EXE_LINKER_FLAGS_ASAN
            "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
            "Linker flags to be used to create executables for Asan build type." FORCE)

        set(CMAKE_SHARED_LINKER_FLAGS_ASAN
            "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address" CACHE STRING
            "Linker lags to be used to create shared libraries for Asan build type." FORCE)
    endif()
endmacro()


function(define_test_program name)
    set(define_test_program_PARAM_OPTIONS)
    set(define_test_program_PARAM_ONE_VALUE_KEYWORDS
        # MAIN_LIB
        PREFIX
        CXXSTANDARD # such as CXXSTANDARD 17
        VERSION # default is ${PROJECT_VERSION}
    )
    set(define_test_program_PARAM_MULTI_VALUE_KEYWORDS
        CXXFLAGS # cxx compiler options
        CXXDEFINITIONS
        LINKFLAGS
        HEADERS
        DETAILED_HEADERS
        SOURCES
        INCLUDE_DIRECTORIES
        LIBRARIES
        FLEX
        BISON
    )

    cmake_parse_arguments(
        define_test_program_ARG
        "${define_test_program_PARAM_OPTIONS}"
        "${define_test_program_PARAM_ONE_VALUE_KEYWORDS}"
        "${define_test_program_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )
    set(define_test_program_usage "define_test_programe(<Name>
     [CXXSTANDARD 17/20/11]
     [CXXFLAGS -Wall [...]]
	 [CXXDEFINITIONS A=1 B C=TRUE [...]]
     [VERSION \"\${PROJECT_VERSION}\"]
     [HEADERS a.hh b.hh ...]
     [DETAILED_HEADERS detail/a.hh...]
     [SOURCES a.cc b.cc ...]
     [INCLUDE_DIRECTORIES ../include ./include ...]
     [LIBRARIES rt thread ...]
     [FLEX <my-flex-target> <scanner.l> [options...]]
     [BISON <my-bison-target> <parser.l> [options...]]
     [sources...]
     )
     
     prefix-name should be UPPERCASE since it's primary used as part of C/C++ Macro Name.
     
     Sample for unpositional param [sources...]:
        define_test_program(foo a.cc b.cc SOURCES c.cc)
        # it declares 3 sources: a.cc b.cc c.cc
     
     Unparsed Params Are:
     ${define_test_program_ARG_UNPARSED_ARGUMENTS}"    )

    #if (NOT "${define_test_program_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
    #    message(SEND_ERROR ${define_test_program_usage})
    #else ()

    #set(_src_list "${define_test_program_MULTI_VALUE_KEYWORDS}")
    set(_src_list ${define_test_program_ARG_SOURCES})
    foreach(f ${define_test_program_ARG_UNPARSED_ARGUMENTS})
        list(APPEND _src_list ${f})
    endforeach()

    # debug_print_list_value(define_test_program_ARG_LIBRARIES)
    # debug_print_list_value(_src_list)

    if(NOT "${define_test_program_ARG_PREFIX}" STREQUAL "")
        set(_name_prefix "${define_test_program_ARG_PREFIX}")
    elseif(NOT "${PROJECT_PREFIX}" STREQUAL "")
        set(_name_prefix "${PROJECT_PREFIX}")
    elseif(NOT "${PROJECT_MACRO_NAME}" STREQUAL "")
        set(_name_prefix "${PROJECT_MACRO_NAME}")
    elseif(NOT "${PROJECT_NAME}" STREQUAL "")
        set(_name_prefix "${PROJECT_NAME}")
    endif()
    if(NOT "${_name_prefix}" STREQUAL "")
        set(_proj_name ${_name_prefix}-${name})
    else()
        set(_proj_name ${name})
    endif()

    if(NOT "${PROJECT_MACRO_PREFIX}" STREQUAL "")
        set(_macro_name_prefix "${PROJECT_MACRO_PREFIX}")
    else()
        set(_macro_name_prefix "${_name_prefix}")
    endif()
    ensure_options_values(${_proj_name} ${_macro_name_prefix})
    #debug_print_value(_macro_name_prefix)

    debug("--------- Test Program ---------> ${_proj_name} (prefix: ${_name_prefix}) | SRC: ${_src_list} | LIBS: ${define_test_program_ARG_LIBRARIES}")

    if("${define_test_program_ARG_VERSION}" STREQUAL "")
        set(define_test_program_ARG_VERSION "${PROJECT_VERSION}")
    endif()
    if("${define_test_program_ARG_CXXSTANDARD}" STREQUAL "")
        set(define_test_program_ARG_CXXSTANDARD "17")
    endif()
    # debug_print_value(define_test_program_ARG_CXXSTANDARD)

    list(APPEND define_test_program_ARG_INCLUDE_DIRECTORIES
        "${CMAKE_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/include")

    add_executable(${_proj_name} ${_src_list})

    # debug_print_value(_proj_name)
    add_cxx_standard_to(${_proj_name} ${define_test_program_ARG_CXXSTANDARD})
    #target_compile_features(${_proj_name}-${name} PRIVATE cxx_std_11)
    #target_compile_definitions(${_proj_name}-${name} PRIVATE)
    target_compile_definitions(${_proj_name} PRIVATE
        ${_${_proj_name}CXXDEFS}
        ${define_test_program_ARG_CXXDEFINITIONS}
        #${_macro_name_prefix}_ENABLE_ASSERTIONS=${_${_name_prefix}_enable_assertions}
        #${_macro_name_prefix}_ENABLE_PRECONDITION_CHECKS=${_${_name_prefix}_enable_precondition_checks}
        #${_macro_name_prefix}_ENABLE_THREAD_POOL_READY_SIGNAL=${_${_name_prefix}_enable_thread_pool_ready_signal}
        #${_macro_name_prefix}_ENABLE_VERBOSE_LOG=${_${_name_prefix}_enable_verbose_log}
        #${_macro_name_prefix}_TEST_THREAD_POOL_DBGOUT=${_${_name_prefix}_enable_thread_pool_dbgout}
        #${_macro_name_prefix}_UNIT_TEST=${_${_name_prefix}_unit_test}
        #${_name_prefix}_UNIT_TEST=1
        #UNIT_TESTING=1
    )

    set_target_properties(${_proj_name} PROPERTIES LINKER_LANGUAGE CXX)

    # target_compile_features(${_proj_name} PRIVATE "cxx_std_${define_test_program_ARG_CXXSTANDARD}")

    #target_compile_options(${_proj_name} PRIVATE
    #        # -fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
    #        "${define_test_program_ARG_CXXFLAGS}"
    #        )

    target_include_directories(${_proj_name}
        PUBLIC
        ${define_test_program_ARG_INCLUDE_DIRECTORIES}
        PRIVATE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>

        SYSTEM PRIVATE
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
    )

    string(REPLACE ";" " " CXXFLAGS_STR "${define_test_program_ARG_CXXFLAGS}")
    if(MSVC)
        string(REPLACE " -" " /" CXXFLAGS_STR " ${CXXFLAGS_STR}")
        list(APPEND define_test_program_ARG_CXXFLAGS
            /W4 /WX /utf-8 /wd4800 # truncation to bool warning
            /Zc:__cplusplus
        )
        target_compile_options(${_proj_name} PRIVATE
            # /W4 /WX /utf-8
            # /D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
            ${define_test_program_ARG_CXXFLAGS}
        )
    else()
        if(USING_GCC)
            list(APPEND define_test_program_ARG_CXXFLAGS
                -Wl,--demangle
            )
        endif()
        list(APPEND define_test_program_ARG_CXXFLAGS
            -pedantic -Wall -Wextra -Wshadow -Werror -pthread
        )
        target_compile_options(${_proj_name} PRIVATE
            ${define_test_program_ARG_CXXFLAGS} # ${CXXFLAGS_STR}

            #-fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'

            #-pedantic -Wall -Wextra -Werror=return-type -Wshadow=local -Wempty-body -fdiagnostics-color
            #-D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
        )
        # message(STATUS "[test project: ${_proj_name}] CXXFLAGS: ${CXXFLAGS_STR}")
        if(USE_DEBUG)
            #            target_compile_options(${_proj_name} PRIVATE
            #                    -fsanitize=address -fno-omit-frame-pointer
            #                    # -fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1
            #                    # -fsanitize=leak -fno-omit-frame-pointer -g -O1
            #                    # -fsanitize=thread -g -O1
            #                    # -fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2
            #                    -fsanitize=undefined -fno-sanitize=vptr
            #
            #                    # address(AddressSanitizer), leak(LeakSanitizer), thread(ThreadSanitizer), undefined(UndefinedBehaviorSanitizer), memory(MemorySanitizer) 
            #                    # [additional_options]: -fno-omit-frame-pointer, fsanitize-recover/fno-sanitize-recover, -fsanitize-blacklist, etc.
            #                    # [-g] [-OX]
            #                    )
            #            if (NOT macOS)
            #                target_compile_options(${_proj_name} PRIVATE
            #                        -fsanitize=leak -fno-omit-frame-pointer
            #                        )
            #                target_link_options(${_proj_name} PRIVATE -fsanitize=address)
            #            endif ()
            #            target_link_options(${_proj_name} PRIVATE -fsanitize=address)
        endif()
    endif()

    target_link_libraries(${_proj_name}
        PRIVATE
        Threads::Threads
        ${define_test_program_ARG_LIBRARIES}
        # undo_cxx
        # fsm_cxx
        # cmdr11::cmdr11
        # Catch2::Catch2
        # fmt::fmt-header-only
    )


    if(NOT define_test_program_ARG_FLEX AND NOT define_test_program_ARG_BISON)
    else()
        debug_print_value(define_test_program_ARG_FLEX)
        debug_print_value(define_test_program_ARG_BISON)
        add_lexer_targets_to(${PROJ_NAME}
            FLEX ${define_test_program_ARG_FLEX}
            BISON ${define_test_program_ARG_BISON}
            ${define_test_program_ARG_UNPARSED_ARGUMENTS}
        )
    endif()


    if(${ENABLE_AUTOMATE_TESTS})
        get_property(tmp GLOBAL PROPERTY UNIT_TEST_TARGETS)
        set(tmp ${UNIT_TEST_TARGETS} ${_proj_name})
        set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "${tmp}")
        message(">> add_test(${_proj_name}) ...")
    endif()

    if(ANDROID)
        add_test(NAME ${_proj_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} "-DANDROID_NDK=${ANDROID_NDK}"
            "-DTEST_RESOURCES_DIR=${CMAKE_SOURCE_DIR}"
            "-DTEST_RESOURCES=tests/data;tests/file_data.txt;Makefile"
            "-DUNITTEST=${_proj_name}"
            -P ${CMAKE_CURRENT_SOURCE_DIR}/ExecuteOnAndroid.cmake)
    else()
        add_test(NAME ${_proj_name}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND $<TARGET_FILE:${_proj_name}>)
    endif()
endfunction()


function(define_example_program name)
    set(define_example_program_PARAM_OPTIONS)
    set(define_example_program_PARAM_ONE_VALUE_KEYWORDS
        PREFIX
        CXXSTANDARD # such as CXXSTANDARD 17
        VERSION # default is ${PROJECT_VERSION}
    )
    set(define_example_program_PARAM_MULTI_VALUE_KEYWORDS
        CXXFLAGS # cxx compiler options
        CXXDEFINITIONS
        HEADERS
        DETAILED_HEADERS
        SOURCES
        INCLUDE_DIRECTORIES
        LIBRARIES
        FLEX
        BISON
    )

    cmake_parse_arguments(
        define_example_program_ARG
        "${define_example_program_PARAM_OPTIONS}"
        "${define_example_program_PARAM_ONE_VALUE_KEYWORDS}"
        "${define_example_program_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )
    set(define_example_program_usage "define_example_programe(<Name>
     [CXXSTANDARD 17/20/11]
     [CXXFLAGS -Wall [...]]
	 [CXXDEFINITIONS A=1 B C=TRUE [...]]
     [VERSION \"\${PROJECT_VERSION}\"]
     [HEADERS a.hh b.hh ...]
     [DETAILED_HEADERS detail/a.hh...]
     [SOURCES a.cc b.cc ...]
     [INCLUDE_DIRECTORIES ../include ./include ...]
     [LIBRARIES rt thread ...]
     [FLEX <my-flex-target> <scanner.l> [options...]]
     [BISON <my-bison-target> <parser.l> [options...]]
     [sources...]
     )
     
     prefix-name should be UPPERCASE since it's primary used as part of C/C++ Macro Name.
     
     Sample for unpositional param [sources...]:
        define_example_program(foo a.cc b.cc SOURCES c.cc)
        # it declares 3 sources: a.cc b.cc c.cc
     
     Unparsed Params Are:
     ${define_example_program_ARG_UNPARSED_ARGUMENTS}"    )

    #if (NOT "${define_example_program_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
    #    message(SEND_ERROR ${define_example_program_usage})
    #else ()

    #set(_src_list "${define_example_program_MULTI_VALUE_KEYWORDS}")
    set(_src_list ${define_example_program_ARG_SOURCES})
    foreach(f ${define_example_program_ARG_UNPARSED_ARGUMENTS})
        list(APPEND _src_list ${f})
    endforeach()

    # debug_print_list_value(define_example_program_ARG_LIBRARIES)
    # debug_print_list_value(_src_list)

    if(NOT "${define_example_program_ARG_PREFIX}" STREQUAL "")
        set(_name_prefix "${define_example_program_ARG_PREFIX}")
    elseif(NOT "$PROJECT_NAME" STREQUAL "")
        set(_name_prefix "${PROJECT_NAME}")
    endif()

    if(NOT "${_name_prefix}" STREQUAL "")
        set(_proj_name ${_name_prefix}-${name})
    else()
        set(_proj_name ${name})
    endif()

    # ensure_options_values(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX})
    if(NOT "${PROJECT_MACRO_PREFIX}" STREQUAL "")
        set(_macro_name_prefix "${PROJECT_MACRO_PREFIX}")
    else()
        set(_macro_name_prefix "${_name_prefix}")
    endif()
    ensure_options_values(${_proj_name} ${_macro_name_prefix})

    debug("--------- Example Program ---------> ${_proj_name} (prefix: ${_name_prefix}) | LIBS: ${define_example_program_ARG_LIBRARIES} | SRC: ${_src_list}.")

    if("${define_example_program_ARG_VERSION}" STREQUAL "")
        set(define_example_program_ARG_VERSION "${PROJECT_VERSION}")
    endif()
    if("${define_example_program_ARG_CXXSTANDARD}" STREQUAL "")
        set(define_example_program_ARG_CXXSTANDARD "17")
    endif()
    # debug_print_value(define_example_program_ARG_CXXSTANDARD)

    list(APPEND define_example_program_ARG_INCLUDE_DIRECTORIES
        "${CMAKE_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/include")

    add_executable(${_proj_name} ${_src_list})

    # debug_print_value(_proj_name)
    add_cxx_standard_to(${_proj_name} ${define_example_program_ARG_CXXSTANDARD})
    #target_compile_features(${_proj_name}-${name} PRIVATE cxx_std_11)
    #target_compile_definitions(${_proj_name}-${name} PRIVATE)
    target_compile_definitions(${_proj_name} PRIVATE
        ${_${_proj_name}CXXDEFS}
        ${define_test_program_ARG_CXXDEFINITIONS}
    )

    set_target_properties(${_proj_name} PROPERTIES LINKER_LANGUAGE CXX)

    # target_compile_features(${_proj_name} PRIVATE "cxx_std_${define_example_program_ARG_CXXSTANDARD}")

    #target_compile_options(${_proj_name} PRIVATE
    #        # -fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'
    #        "${define_example_program_ARG_CXXFLAGS}"
    #        )

    target_include_directories(${_proj_name}
        PUBLIC
        ${define_example_program_ARG_INCLUDE_DIRECTORIES}
        PRIVATE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>

        SYSTEM PRIVATE
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
    )

    string(REPLACE ";" " " CXXFLAGS_STR "${define_test_program_ARG_CXXFLAGS}")
    if(MSVC)
        string(REPLACE " -" " /" CXXFLAGS_STR " ${CXXFLAGS_STR}")
        list(APPEND define_example_program_ARG_CXXFLAGS
            /W4 /WX /utf-8 /wd4800 # truncation to bool warning
            /Zc:__cplusplus
        )
        target_compile_options(${_proj_name} PRIVATE
            #/W4 /WX /utf-8
            #/D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
            ${define_example_program_ARG_CXXFLAGS}
        )
    else()
        if(USING_GCC)
            list(APPEND define_example_program_ARG_CXXFLAGS
                -Wl,--demangle
            )
        endif()
        list(APPEND define_example_program_ARG_CXXFLAGS
            -pedantic -Wall -Wextra -Wshadow -Werror -pthread
        )
        target_compile_options(${_proj_name} PRIVATE
            ${define_example_program_ARG_CXXFLAGS}

            #-fno-permissive # don't care about declaration of '<name>' changes meaning of '<name>'

            #-pedantic -Wall -Wextra -Werror=return-type -Wshadow=local -Wempty-body -fdiagnostics-color
            #-D${PROJECT_MACRO_PREFIX}_UNIT_TEST=${_${PROJECT_MACRO_NAME}_unit_test}
        )
        # message(STATUS "[example-prog] CXXFLAGS: ${define_example_program_ARG_CXXFLAGS}")
        if(USE_DEBUG)
            #            target_compile_options(${_proj_name} PRIVATE
            #                    -fsanitize=address -fno-omit-frame-pointer
            #                    # -fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1
            #                    # -fsanitize=leak -fno-omit-frame-pointer -g -O1
            #                    # -fsanitize=thread -g -O1
            #                    # -fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2
            #                    -fsanitize=undefined -fno-sanitize=vptr
            #
            #                    # address(AddressSanitizer), leak(LeakSanitizer), thread(ThreadSanitizer), undefined(UndefinedBehaviorSanitizer), memory(MemorySanitizer) 
            #                    # [additional_options]: -fno-omit-frame-pointer, fsanitize-recover/fno-sanitize-recover, -fsanitize-blacklist, etc.
            #                    # [-g] [-OX]
            #                    )
            #            if (NOT macOS)
            #                target_compile_options(${_proj_name} PRIVATE
            #                        -fsanitize=leak -fno-omit-frame-pointer
            #                        )
            #                target_link_options(${_proj_name} PRIVATE -fsanitize=address)
            #            endif ()
            #            target_link_options(${_proj_name} PRIVATE -fsanitize=address)
        endif()
    endif()
    target_link_libraries(${_proj_name}
        PRIVATE
        Threads::Threads
        ${define_example_program_ARG_LIBRARIES}
        # undo_cxx
        # fsm_cxx
        # cmdr11::cmdr11
        # Catch2::Catch2
        # fmt::fmt-header-only
    )


    if(NOT define_example_program_ARG_FLEX AND NOT define_example_program_ARG_BISON)
    else()
        debug_print_value(define_example_program_ARG_FLEX)
        debug_print_value(define_example_program_ARG_BISON)
        add_lexer_targets_to(${PROJ_NAME}
            FLEX ${define_example_program_ARG_FLEX}
            BISON ${define_example_program_ARG_BISON}
            ${define_example_program_ARG_UNPARSED_ARGUMENTS}
        )
    endif()

endfunction()
