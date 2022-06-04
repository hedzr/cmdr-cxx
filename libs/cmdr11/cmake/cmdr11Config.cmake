## Get the directory path of the <target>.cmake file
## get_filename_component(CMDR11_CMAKE_DIR "cmdr11Config.cmake" DIRECTORY)
get_filename_component(CMDR11_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(CMDR11_INCLUDE_DIR "${CMDR11_CMAKE_DIR}/../../../include" REALPATH)

## Add the dependencies of our library
#include(CMakeFindDependencyMacro)
##find_dependency(ZLIB REQUIRED)
##find_dependency(fmt REQUIRED)


# Compute paths
#get_filename_component(CMDR_CXX_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
#set(CMDR11_INCLUDE_DIR "${CMDR11_CMAKE_DIR}/../../../include")


## Import the targets
if (NOT TARGET cmdr11::cmdr11)
    #include("${SM_LIB_CMAKE_DIR}/cmdr11Targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/cmdr11Targets.cmake")

    # These are IMPORTED targets created by cmdr11Targets.cmake
    set(CMDR11_LIBRARIES "cmdr11::cmdr11")

    if (EXISTS ${CMDR11_INCLUDE_DIR}/cmdr11/cmdr11.hh)
        set(CMDR11_FOUND ON)

        set(CMDR11_INCLUDE_DIRS ${CMDR11_INCLUDE_DIR})
        get_filename_component(CMDR11_LIBRARY_DIR "${CMDR11_CMAKE_DIR}/../../../lib" REALPATH)
        set(CMDR11_LIBRARY_DIRS ${CMDR11_LIBRARY_DIR})

        file(READ "${CMDR11_INCLUDE_DIR}/cmdr11/cmdr-version.hh" ver)

        string(REGEX MATCH "MAJOR_VERSION +([0-9]+)" _ ${ver})
        set(CMDR11_VERSION_MAJOR ${CMAKE_MATCH_1})

        string(REGEX MATCH "MINOR_VERSION +([0-9]+)" _ ${ver})
        set(CMDR11_VERSION_MINOR ${CMAKE_MATCH_1})

        string(REGEX MATCH "PATCH_NUMBER +([0-9]+)" _ ${ver})
        set(CMDR11_VERSION_PATCH ${CMAKE_MATCH_1})

        string(REGEX MATCH "CMDR_VERSION_STRING +xT\\(\\\"([0-9.]+)\\\"\\)" _ ${ver})
        set(CMDR11_VERSION_STRING ${CMAKE_MATCH_1})

        string(REGEX MATCH "CMDR_GIT_COMMIT_HASH +xT\\(\\\"([0-9a-f.-]+)\\\"\\)" _ ${ver})
        set(CMDR11_VERSION_GIT_HASH ${CMAKE_MATCH_1})

        message(">=< CMDR_VERSION        = ${CMDR11_VERSION_MAJOR}.${CMDR11_VERSION_MINOR}.${CMDR11_VERSION_PATCH}")
        message(">=< CMDR_VERSION_STRING = ${CMDR11_VERSION_STRING} (HASH: ${CMDR11_VERSION_GIT_HASH})")
        message(">=< CMDR11_INCLUDE_DIR  = ${CMDR11_INCLUDE_DIR}")
        message(">=< CMDR11_LIBRARY_DIR  = ${CMDR11_LIBRARY_DIR}")
        message(">=< CMDR11_LIBRARIES    = ${CMDR11_LIBRARIES}")
    endif ()

endif ()
