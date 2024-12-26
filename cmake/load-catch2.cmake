# https://cmake.org/cmake/help/latest/module/FetchContent.html#integrating-with-find-package

# https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md
# find_package(Catch2 3 REQUIRED)
find_package(Catch2 3) # want Catch2 v3.x

if (Catch2_FOUND)
	# if you've install Catch2 v3.x via package manager, such as:
	#    macOS: brew install catch2
	#    debian/ubuntu: sudo apt-get install catch2
	#    opensuse: sudo zypper in Catch2-devel
	debug("Catch2 ${Catch2_VERSION} found.")
else ()
	include(FetchContent)
	
	debug("try loading catch2 v3.x library ...")
	
	# FetchContent_Declare(
	# googletest
	# GIT_REPOSITORY https://github.com/google/googletest.git
	# GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
	# FIND_PACKAGE_ARGS NAMES GTest
	# )
	FetchContent_Declare(
			Catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG fa43b77429ba76c462b1898d6cd2f2d7a9416b14 # v3.7.1
			# GIT_TAG 605a34765aa5d5ecbf476b4598a862ada971b0cc # v3.0.1
			FIND_PACKAGE_ARGS
	)
	
	# This will try calling find_package() first for both dependencies
	# FetchContent_MakeAvailable(googletest Catch2)
	FetchContent_MakeAvailable(Catch2)
endif ()