
#find_package(Catch2)
#if (NOT CATCH2_FOUND)
include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.8.2)

FetchContent_MakeAvailable(spdlog)
#endif ()
