# https://cmake.org/cmake/help/latest/module/FetchContent.html#integrating-with-find-package

include(FetchContent)

FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG 703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
		FIND_PACKAGE_ARGS NAMES GTest
)

# FetchContent_Declare(
# Catch2
# GIT_REPOSITORY https://github.com/catchorg/Catch2.git
# GIT_TAG        605a34765aa5d5ecbf476b4598a862ada971b0cc # v3.0.1
# FIND_PACKAGE_ARGS
# )

# This will try calling find_package() first for both dependencies
# FetchContent_MakeAvailable(googletest Catch2)
FetchContent_MakeAvailable(googletest)
