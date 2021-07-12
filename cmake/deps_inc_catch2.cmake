
find_package(Catch2)
message(STATUS "Catch2_FOUND = ${Catch2_FOUND}")
message(STATUS "Catch2_VERSION = ${Catch2_VERSION}")
if (NOT Catch2_FOUND)
    include(FetchContent)

    message("fetch Catch2 from github ...")
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v2.13.3)

    message(STATUS "Downloading Catch2 and make it available (FetchContent) ")
    FetchContent_MakeAvailable(Catch2)
endif ()
