
find_package(Catch2)
if (NOT CATCH2_FOUND)
    include(FetchContent)

    message("fetch Catch2 from github ...")
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v2.13.3)

    message(STATUS "Downloading Catch2 and make it available (FetchContent) ")
    FetchContent_MakeAvailable(Catch2)
endif ()
