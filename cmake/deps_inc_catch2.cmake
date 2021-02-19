
#find_package(Catch2)
#if (NOT CATCH2_FOUND)
include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.3)

FetchContent_MakeAvailable(Catch2)
#endif ()
