#
# /usr/local/share/cmake/Modules/FindBISON.cmake
# /usr/local/share/cmake/Modules/FindFLEX.cmake
#
# /usr/local/opt/cmake/Modules/FindBISON.cmake
# /usr/local/opt/cmake/Modules/FindFLEX.cmake
#
# /Applications/CLion.app/Contents/bin/cmake/mac/share/cmake-3.24/Modules/FindFLEX.cmake
# /Applications/CLion.app/Contents/bin/cmake/mac/share/cmake-3.24/Modules/FindBISON.cmake
#

macro(find_lexer FlexMinVer BisonMinVer)
    # On macOS, search Homebrew for keg-only versions of Bison and Flex. Xcode does
    # not provide new enough versions for us to use.
    # https://stackoverflow.com/questions/53877344/cannot-configure-cmake-to-look-for-homebrew-installed-version-of-bison
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
        execute_process(
            COMMAND brew --prefix bison
            RESULT_VARIABLE BREW_BISON
            OUTPUT_VARIABLE BREW_BISON_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(BREW_BISON EQUAL 0 AND EXISTS "${BREW_BISON_PREFIX}")
            message(STATUS "Found Bison keg installed by Homebrew at ${BREW_BISON_PREFIX}")
            set(BISON_EXECUTABLE "${BREW_BISON_PREFIX}/bin/bison")
        endif()

        execute_process(
            COMMAND brew --prefix flex
            RESULT_VARIABLE BREW_FLEX
            OUTPUT_VARIABLE BREW_FLEX_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(BREW_FLEX EQUAL 0 AND EXISTS "${BREW_FLEX_PREFIX}")
            message(STATUS "Found Flex keg installed by Homebrew at ${BREW_FLEX_PREFIX}")
            set(FLEX_EXECUTABLE "${BREW_FLEX_PREFIX}/bin/flex")
            set(FLEX_INCLUDE_DIR "${BREW_FLEX_PREFIX}/include")
            set(FL_LIBRARY "${BREW_FLEX_PREFIX}/lib")
        endif()
    endif()

    # FIND_PACKAGE(FLEX 2.6)
    # FIND_PACKAGE(BISON 2.7)
    find_package(FLEX ${FlexMinVer}) # 2.6.4
    find_package(BISON ${BisonMinVer}) # 3.8.2
    if(BISON_FOUND)
        message(STATUS "- BISON ${BISON_VERSION} FOUND: ${BISON_EXECUTABLE} - BUILT FOR ${BisonTarget}")
    else()
        message(FATAL_ERROR "need bison")
    endif()

    if(FLEX_FOUND)
        message(STATUS "- FLEX ${FLEX_VERSION} FOUND: ${FLEX_EXECUTABLE}, inc: ${FLEX_INCLUDE_DIR} - BUILT FOR ${FlexTarget}")

        # debug_print_value(FLEX_LIBRARIES)
        # debug_print_list_value(FLEX_INCLUDE_DIRS)
    else()
        message(FATAL_ERROR "need flex")
    endif()
endmacro()

macro(add_lexer_targets_to Target) # FlexTarget FlexInput BisonTarget BisonInput)
    set(add_lexer_targets_to_PARAM_OPTIONS)
    set(add_lexer_targets_to_PARAM_ONE_VALUE_KEYWORDS
    )
    set(add_lexer_targets_to_PARAM_MULTI_VALUE_KEYWORDS
        FLEX
        BISON
    )

    cmake_parse_arguments(
        add_lexer_targets_to_ARG
        "${add_lexer_targets_to_PARAM_OPTIONS}"
        "${add_lexer_targets_to_PARAM_ONE_VALUE_KEYWORDS}"
        "${add_lexer_targets_to_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )
    set(add_lexer_targets_to_usage "add_lexer_targets_to(<Name>
     [FLEX <my-flex-target> <scanner.l> [options...]]
     [BISON <my-bison-target> <parser.l> [options...]]
     source_files...
     )
     "    )

    # if (NOT "${add_lexer_targets_to_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
    # message(SEND_ERROR ${add_lexer_targets_to_usage})
    # else ()

    # # set(_src_list "${add_lexer_targets_to_MULTI_VALUE_KEYWORDS}")
    # set(_src_list)
    # foreach (f ${add_lexer_targets_to_ARG_UNPARSED_ARGUMENTS})
    # list(APPEND _src_list ${f})
    # endforeach ()
    list(GET add_lexer_targets_to_ARG_FLEX 0 FlexTarget)
    list(GET add_lexer_targets_to_ARG_FLEX 1 FlexInput)
    list(REMOVE_AT add_lexer_targets_to_ARG_FLEX 0 1)
    set(FlexOptions "${add_lexer_targets_to_ARG_FLEX}")
    list(GET add_lexer_targets_to_ARG_BISON 0 BisonTarget)
    list(GET add_lexer_targets_to_ARG_BISON 1 BisonInput)
    list(REMOVE_AT add_lexer_targets_to_ARG_BISON 0 1)
    set(BisonOptions "${add_lexer_targets_to_ARG_BISON}")

    list(APPEND FlexOptions
    )
    list(APPEND BisonOptions
        --html --graph -Wcounterexamples
    )

    string(REPLACE ";" " " FLEX_FLAGS "${FlexOptions}")
    string(REPLACE ";" " " BISON_FLAGS "${BisonOptions}")

    # debug_print_value(FlexTarget)
    # debug_print_value(FlexInput)
    # debug_print_value(FlexOptions)
    # debug_print_value(BisonTarget)
    # debug_print_value(BisonInput)
    # debug_print_value(BISON_FLAGS)
    # message(STATUS "<***>")

    #
    #
    #
    if(BISON_FOUND)
        if(APPLE)
            bison_target(
                ${BisonTarget}
                ${BisonInput}
                ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.cc
                COMPILE_FLAGS "${BISON_FLAGS}" # here is bison options # -Wconflicts-sr -Wconflicts-rr

                # DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.defs.hh
                VERBOSE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.verbose.lst
                REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.report.lst
            )
        else()
            bison_target(
                ${BisonTarget}
                ${BisonInput}
                ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.cc
                COMPILE_FLAGS "${BISON_FLAGS}" # here is bison options # -Wconflicts-sr -Wconflicts-rr

                # DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.defs.hh
                # VERBOSE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.verbose.lst
                # REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.report.lst
            )
        endif()

        if(FLEX_FOUND)
            flex_target(
                ${FlexTarget}
                ${FlexInput}
                ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.cc
                COMPILE_FLAGS "${FLEX_FLAGS}" # here is flex options
                DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.def.hh
            )

            # message(STATUS "  OUT: FLEX.DEFINES_FILE = ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.def.hh")
            # message(STATUS "  OUT: FLEX.TAB.cc       = ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.cc")
            add_flex_bison_dependency(${FlexTarget} ${BisonTarget})
        else()
            message(FATAL_ERROR " flex needed.")
        endif()

        # message(STATUS "  OUT: BISON.cc          = ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.cc")
        # message(STATUS "  OUT: BISON.hh          = ${CMAKE_CURRENT_BINARY_DIR}/${BisonInput}.hh")
        # message(STATUS "  OUT: BISON.verbose     = ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.verbose.list")
        # message(STATUS "  OUT: BISON.report      = ${CMAKE_CURRENT_BINARY_DIR}/${FlexInput}.report.list")
        list(APPEND LEX_OPTS
            -DYYDEBUG=1
            -Wno-error=conversion -Wno-conversion -Wno-sign-conversion -Wno-sign-compare
            -Wno-unused-function
            -Wno-unused-but-set-variable

            # -Wno-error=implicit-function-declaration
            # -Wno-error=unused-function
            # -Wno-int-conversion
        )

        if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
            list(APPEND LEX_OPTS -Wno-implicit-function-declaration)
        endif()

        target_compile_options(${Target} PUBLIC
            ${LEX_OPTS}

            # -Wunused-but-set-variable
        )
        target_include_directories(${Target} PRIVATE
            ${FLEX_INCLUDE_DIRS}
            ${BISON_INCLUDE_DIRS}
        )

        # target_link_directories(${TARGET} PRIVATE)
        # SET_SOURCE_FILES_PROPERTIES(parser.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion -Wno-sign-conversion")
        # ADD_FLEX_BISON_DEPENDENCY(${FlexTarget} ${BisonTarget})

        # message(STATUS "FLEX_${FlexTarget}_DEFINED: ${FLEX_${FlexTarget}_DEFINED}")
        # message(STATUS "FLEX_${FlexTarget}_OUTPUTS: ${FLEX_${FlexTarget}_OUTPUTS}")
        # message(STATUS "BISON_${BisonTarget}_DEFINED: ${BISON_${BisonTarget}_DEFINED}")
        # message(STATUS "BISON_${BisonTarget}_OUTPUTS: ${BISON_${BisonTarget}_OUTPUTS}")
        target_sources(${Target} PRIVATE
            ${BISON_${BisonTarget}_OUTPUTS}
            ${FLEX_${FlexTarget}_OUTPUTS}
        )
        target_include_directories(${Target} PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
        )

    else()
        message(FATAL_ERROR "flex and bison needed.")
    endif()
endmacro()
