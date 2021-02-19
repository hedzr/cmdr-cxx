

# https://github.com/pmirshad/cmake-with-git-metadata/blob/master/CMakeLists.txt


if (DEFINED VERSION_H_NAME)
else ()
  set(VERSION_H_NAME "version.h")
endif ()

if (DEFINED ARCHIVE_NAME)
else ()
  set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
endif ()

if (DEFINED xVERSION_IN)
else ()
  set(xVERSION_IN ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in)
endif ()
message("Using version.in file: ${xVERSION_IN}, ARCHIVE_NAME = ${ARCHIVE_NAME}")

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process(
          COMMAND git rev-parse --abbrev-ref HEAD
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
          OUTPUT_VARIABLE GIT_BRANCH
          OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
          COMMAND git log -1 --format=%h
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
          OUTPUT_VARIABLE GIT_COMMIT_HASH
          OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(GIT_BRANCH "")
  set(GIT_COMMIT_HASH "")
endif (EXISTS "${CMAKE_SOURCE_DIR}/.git")

# get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_COMMIT_HASH}" 0 12 GIT_REV)
if (NOT GIT_COMMIT_HASH)
  set(GIT_REV "0")
endif ()

message(STATUS "- Git current branch: ${GIT_BRANCH}")
message(STATUS "- Git commit hash:    ${GIT_COMMIT_HASH}")
message(STATUS "- Git rev:            ${GIT_REV}")

if (CMAKE_GENERATED_DIR)
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






