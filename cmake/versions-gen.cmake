# https://github.com/pmirshad/cmake-with-git-metadata/blob/master/CMakeLists.txt

macro(gen_versions PROJ_NAME PROJECT_MACRO_PREFIX VERSION_H_NAME CONFIG_H_NAME ARCHIVE_NAME xVERSION_IN xCONFIG_BASE_IN)
    if (DEFINED PROJ_NAME)
    else ()
        set(PROJ_NAME ${CMAKE_PROJECT_NAME})
    endif ()

    if (DEFINED PROJECT_MACRO_PREFIX)
    else ()
        set(PROJECT_MACRO_PREFIX ${PROJ_NAME})
    endif ()

    if (DEFINED VERSION_H_NAME)
    else ()
        set(VERSION_H_NAME "${PROJ_NAME}-version.hh")
    endif ()

    if (DEFINED CONFIG_H_NAME)
    else ()
        set(CONFIG_H_NAME "${PROJ_NAME}-config.hh")
    endif ()

    if (DEFINED ARCHIVE_NAME)
    else ()
        set(ARCHIVE_NAME ${PROJ_NAME}-${CMAKE_PROJECT_VERSION})
    endif ()

    if (DEFINED xVERSION_IN)
    else ()
        set(xVERSION_IN ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in)
    endif ()

    if (DEFINED xCONFIG_BASE_IN)
    else ()
        set(xCONFIG_BASE_IN ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in)
    endif ()

    set(xOUT_DIR ${CMAKE_GENERATED_DIR})

    message("|| gen_version()           : output-dir -> ${xOUT_DIR}")
    message("||   Using version.in file : ${xVERSION_IN}, ARCHIVE_NAME = ${ARCHIVE_NAME}, PROJECT_MACRO_PREFIX = ${PROJECT_MACRO_PREFIX}")
    message("||                           CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

    if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
        # git describe --tags --abbrev=0   # 0.1.0-dev
        # git describe --tags              # 0.1.0-dev-93-g1416689
        # git describe --abbrev=0          # to get the most recent annotated tag
        execute_process(
            COMMAND git describe --abbrev=0 --tags
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_LAST_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND git describe --tags
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_LAST_TAG_LONG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND git rev-parse HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND git log -1 --format=%h
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_REV
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    else ()
        set(GIT_BRANCH "master")
        set(GIT_LAST_TAG "HEAD")
        set(GIT_LAST_TAG_LONG "HEAD")
        set(GIT_COMMIT_HASH "")
        set(GIT_COMMIT_REV "")
    endif ()

    # # get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    # string(SUBSTRING "${GIT_COMMIT_HASH}" 0 12 GIT_COMMIT_REV)
    # if (NOT GIT_COMMIT_HASH)
    # set(GIT_COMMIT_REV "0")
    # endif ()
    message("||      Git current branch : ${GIT_BRANCH}")
    message("||           Git last tags : ${GIT_LAST_TAG}, Long: ${GIT_LAST_TAG_LONG}")
    message("||         Git commit hash : ${GIT_COMMIT_HASH}, revision: ${GIT_COMMIT_REV}")

    if (NOT "${xOUT_DIR}")
    else ()
        message(FATAL "     >> ERROR: please include target-dirs.cmake at first.")

        # we need CMAKE_GENERATED_DIR at present.
    endif ()

    # include(CheckIncludeFile)
    # include(CheckIncludeFiles)
    # set(HAS_UNISTD_H 0)
    # check_include_file("unistd.h" HAS_UNISTD_H)
    # check_include_files("stdio.h;string.h" HAVE_STDIO_AND_STRING_H)
    message("||        unistd.h checked : HAS_UNISTD_H = ${HAS_UNISTD_H}")

    set(_output_dir ${xOUT_DIR})
    # set(_output_dir ${CMAKE_CURRENT_BINARY_DIR})

    if (EXISTS ${xVERSION_IN})
        message("||    Generating version.h from ${xVERSION_IN} to ${_output_dir} - Version ${PROJECT_VERSION}...")
        configure_file(
            ${xVERSION_IN}
            ${_output_dir}/${VERSION_H_NAME}
        )
        message("|| Generated: ${_output_dir}/${VERSION_H_NAME}")
    endif ()

    if (EXISTS ${xCONFIG_BASE_IN})
        message("||    Generating ${CONFIG_H_NAME} from ${xCONFIG_BASE_IN} to ${_output_dir} - Version ${PROJECT_VERSION}...")
        configure_file(
            ${xCONFIG_BASE_IN}
            ${_output_dir}/${CONFIG_H_NAME}
        )
        message("|| Generated: ${_output_dir}/${CONFIG_H_NAME}")
    endif ()
endmacro()
