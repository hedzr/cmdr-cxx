#
# check CPU bits
#

project(dummy
		DESCRIPTION "dummy-project for sizeof_void_p detection and more."
		LANGUAGES CXX)

if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	# 64 bits
	set(CPU_ARCH "64" CACHE STRING "ARCH x86_64/arm64/..." FORCE)
	
	# set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
	message(STATUS "CPU_ARCH 64bit | ${CMAKE_HOST_SYSTEM_PROCESSOR} ...")
elseif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
	# 32 bits
	set(CPU_ARCH "32" CACHE STRING "ARCH x86" FORCE)
	
	# set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
	message(STATUS "CPU_ARCH 32bit | ${CMAKE_HOST_SYSTEM_PROCESSOR} ...")
else ()
	# message(STATUS "Unknown CMAKE_SIZEOF_VOID_P = '${CMAKE_SIZEOF_VOID_P}' ..?")
	if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "(amd64)|(AMD64)|(x86_64)|(arm64)|(aarch64)")
		set(CPU_ARCH "64" CACHE STRING "ARCH x86_64" FORCE)
		
		# set(CPU_ARCH_NAME "x86_64" CACHE STRING "ARCH x86_64" FORCE)
		# set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
		message(STATUS "CPU_ARCH 64bit | ${CMAKE_HOST_SYSTEM_PROCESSOR} ...")
	else ()
		set(CPU_ARCH "32" CACHE STRING "ARCH x86" FORCE)
		
		# set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
		message(STATUS "CPU_ARCH 32bit | ${CMAKE_HOST_SYSTEM_PROCESSOR} ...")
	endif ()
endif ()

set(CPU_NAME ${CMAKE_HOST_SYSTEM_PROCESSOR})

#
#
#
include(target-dirs)
include(cxx-detect-compilers) # detect cxx compiler
include(detect-systems) # detect OS, ...
include(cxx-macros) # macros: define_cxx_executable_project, ...
include(version-def) # load .version.cmake
include(versions-gen) # generate config.h and version.hh

# when using compiler with cmake multi-config feature, a special build type 'Asan' can be used for sanitizing test.
enable_sanitizer_for_multi_config()
