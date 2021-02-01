
#find_package(Catch2)
#if (NOT CATCH2_FOUND)
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 7.1.3)

FetchContent_MakeAvailable(fmt)
#endif ()
