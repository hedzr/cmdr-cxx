# Set policies
# https://www.mankier.com/7/cmake-policies

# CMake Warning (dev) at CMakeLists.txt:5 (ADD_EXECUTABLE):
# Policy CMP0049 is not set: Do not expand variables in target source
# entries.  Run "cmake --help-policy CMP0049" for policy details.  Use the
# cmake_policy command to set the policy and suppress this warning.
if (POLICY CMP0049)
	cmake_policy(SET CMP0049 NEW)
endif ()

if (COMMAND cmake_policy)
	# we prefer the more strict behavior, to find out more:
	# cmake --help-policy CMP0003
	cmake_policy(SET CMP0003 NEW)
endif ()

# Compiler id for Apple Clang is now AppleClang.
if (POLICY CMP0025)
	cmake_policy(SET CMP0025 NEW)
endif (POLICY CMP0025)

# MACOSX_RPATH is enabled by default.
if (POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif ()

# Project version variables are the empty string if version is unspecified
if (POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

# Only interpret if() arguments as variables or keywords when unquoted.
if (POLICY CMP0054)
	cmake_policy(SET CMP0054 NEW)
endif ()

# if command can use IN_LIST
if (POLICY CMP0057)
	cmake_policy(SET CMP0057 NEW)
endif (POLICY CMP0057)

# Honor visibility properties for all target types.
if (POLICY CMP0063)
	cmake_policy(SET CMP0063 NEW)
endif ()

if (POLICY CMP0068)
	cmake_policy(SET CMP0068 NEW)
endif ()

if (POLICY CMP0069)
	cmake_policy(SET CMP0069 NEW)
endif ()

# if (POLICY CMP0070)
# cmake_policy(SET CMP0070 NEW)
# endif ()
if (POLICY CMP0076)
	cmake_policy(SET CMP0076 NEW) # target_sources() command converts relative paths to absolute
endif ()

# option() honors normal variables
if (POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif (POLICY CMP0077)

# if (POLICY CMP0086)
# # UseSWIG honors SWIG_MODULE_NAME via -module flag
# cmake_policy(SET CMP0086 NEW)
# endif ()

# Allow the user to specify the MSVC runtime
if (POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

if (POLICY CMP0120)
	cmake_policy(SET CMP0120 NEW) # The WriteCompilerDetectionHeader module is removed
endif ()

if (POLICY CMP0068)
    cmake_policy(SET CMP0068 NEW)
endif ()


if ("${CMAKE_VERSION}" VERSION_LESS "3.12")
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()
