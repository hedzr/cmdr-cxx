

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
    message("Using version.in file: ${xVERSION_IN}, ARCHIVE_NAME = ${ARCHIVE_NAME}, PROJECT_MACRO_PREFIX = ${PROJECT_MACRO_PREFIX}")

    if (EXISTS "${CMAKE_SOURCE_DIR}/.git")

        # git describe --tags --abbrev=0   # 0.1.0-dev
        # git describe --tags              # 0.1.0-dev-93-g1416689
        # git describe --abbrev=0          # to get the most recent annotated tag
        execute_process(
                COMMAND git describe --abbrev=0 --tags
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_LAST_TAG
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND git describe --tags
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_LAST_TAG_LONG
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
                COMMAND git rev-parse --abbrev-ref HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
                COMMAND git rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
                COMMAND git log -1 --format=%h
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_COMMIT_REV
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else ()
        set(GIT_BRANCH "")
        set(GIT_LAST_TAG "")
        set(GIT_LAST_TAG_LONG "")
        set(GIT_COMMIT_HASH "")
        set(GIT_COMMIT_REV "")
    endif ()

    #    # get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    #    string(SUBSTRING "${GIT_COMMIT_HASH}" 0 12 GIT_COMMIT_REV)
    #    if (NOT GIT_COMMIT_HASH)
    #        set(GIT_COMMIT_REV "0")
    #    endif ()

    message(STATUS "- Git current branch:  ${GIT_BRANCH}")
    message(STATUS "- Git last tag:        ${GIT_LAST_TAG}")
    message(STATUS "- Git commit hash:     ${GIT_COMMIT_HASH}")
    message(STATUS "- Git commit revision: ${GIT_COMMIT_REV}")

    if (NOT "${CMAKE_GENERATED_DIR}")
    else ()
        message(FATAL " >> ERROR: please include target-dirs.cmake at first.")
        # we need CMAKE_GENERATED_DIR at present.
    endif ()

    if (EXISTS ${xVERSION_IN})
        message(STATUS "Generating version.h from ${xVERSION_IN} to ${CMAKE_GENERATED_DIR} - Version ${PROJECT_VERSION}...")
        configure_file(
                ${xVERSION_IN}
                ${CMAKE_GENERATED_DIR}/${VERSION_H_NAME}
        )
        message(STATUS "Generated: ${CMAKE_GENERATED_DIR}/${VERSION_H_NAME}")
    endif ()

    if (EXISTS ${xCONFIG_BASE_IN})
        message(STATUS "Generating ${CONFIG_H_NAME} from ${xCONFIG_BASE_IN} to ${CMAKE_GENERATED_DIR} - Version ${PROJECT_VERSION}...")
        configure_file(
                ${xCONFIG_BASE_IN}
                ${CMAKE_GENERATED_DIR}/${CONFIG_H_NAME}
        )
        message(STATUS "Generated: ${CMAKE_GENERATED_DIR}/${CONFIG_H_NAME}")
    endif ()


endmacro()




