include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckCCompilerFlag)
include(CheckCXXSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckLibraryExists)

enable_language(CXX C)

# https://cmake.org/cmake/help/latest/module/WriteCompilerDetectionHeader.html
# include(WriteCompilerDetectionHeader)
# https://cmake.org/cmake/help/v3.14/manual/cmake-compile-features.7.html#manual:cmake-compile-features(7)
set(WriterCompilerDetectionHeaderFound NOTFOUND)

# This module is only available with CMake >=3.1, so check whether it could be found
# BUT in CMake 3.1 this module doesn't recognize AppleClang as compiler, so just use it as of CMake 3.2
if ("${CMAKE_VERSION}" VERSION_GREATER "3.2")
	if ("${CMAKE_VERSION}" VERSION_LESS "3.19")
		if (WriterCompilerDetectionHeaderFound)
			include(WriteCompilerDetectionHeader OPTIONAL RESULT_VARIABLE WriterCompilerDetectionHeaderFound)
		endif ()
	endif ()
endif ()

if (WriterCompilerDetectionHeaderFound)
	write_compiler_detection_header(
			FILE ${CMAKE_GENERATED_DIR}/the-compiler.h
			PREFIX The
			COMPILERS GNU Clang MSVC Intel AppleClang # SunPro
			FEATURES
			cxx_variadic_templates cxx_final cxx_override
			cxx_static_assert cxx_constexpr cxx_extern_templates
			cxx_noexcept cxx_thread_local
	)
endif ()

# string(REGEX MATCH ".*Clang.*" USING_CLANG ${CMAKE_CXX_COMPILER_ID})
if (CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
	# using regular Clang or AppleClang
	SET(USING_CLANG ON)
	
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
			# using clang with clang-cl front end
			SET(USING_CLANG_FRONTEND_CL ON)
		elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
			# using clang with regular front end
			SET(USING_CLANG_FRONTEND_GNU ON)
		else ()
			SET(USING_CLANG_FRONTEND_OTHERS ON)
		endif ()
		
		SET(USING_CLANG_LLVM ON)
	else ()
		SET(USING_CLANG_APPLE ON)
	endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# using GCC
	SET(USING_GCC ON)
	SET(USING_GNU ON)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	# using Intel C++
	SET(USING_Intel ON)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# using Visual Studio C++
	SET(USING_MSVC ON)
	add_compile_definitions(WIN32)
else ()
	SET(USING_UNKNOWN_COMPILER ON)
endif ()

#
# CMAKE_POSITION_INDEPENDENT_CODE
#
include(TestBigEndian)
set(CMAKE_POSITION_INDEPENDENT_CODE 0)
set(USING_POSITION_INDEPENDENT_CODE 0)
set(USING_PIC 0)
set(USING_BIG_ENDIAN 0)

if (NOT (${WIN32}))
	TEST_BIG_ENDIAN(_bigendian)
	
	if ((${CMAKE_SIZEOF_VOID_P} GREATER 4) OR (${_bigendian}))
		message(STATUS ">>> Setting PIC (${CMAKE_CXX_COMPILE_OPTIONS_PIC}) for machine ${CMAKE_HOST_SYSTEM_PROCESSOR}")
		set(CMAKE_POSITION_INDEPENDENT_CODE 1)
		set(USING_POSITION_INDEPENDENT_CODE 1)
		set(USING_PIC 1)
	else ()
		message(STATUS ">>> BigEndian is ${_bigendian} for machine ${CMAKE_HOST_SYSTEM_PROCESSOR}")
	endif ()
	
	set(USING_BIG_ENDIAN ${_bigendian})
else ()
	message(STATUS ">>> BigEndian skipped for machine ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif ()

#
#
#
set(ENABLE_CXX ON)
set(ENABLE_CXX11 ON)

if (ENABLE_CXX)
	check_cxx_symbol_exists(__cplusplus ciso646 SYSTEM_HAS_CXX)
	
	if (SYSTEM_HAS_CXX)
		check_cxx_compiler_flag("-std=c++11" SUPPORTS_STDCXX11)
		
		if (SUPPORTS_STDCXX11)
			# set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
			if (ENABLE_CXX)
				set(ENABLE_CXX11 ON)
			endif ()
		endif ()
		
		check_cxx_compiler_flag("-std=c++17" SUPPORTS_STDCXX17)
		
		if (SUPPORTS_STDCXX17)
			# set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
			if (ENABLE_CXX)
				set(ENABLE_CXX17 ON)
			endif ()
		endif ()
		
		check_cxx_compiler_flag("-std=c++20" SUPPORTS_STDCXX20)
		
		if (SUPPORTS_STDCXX20)
			# set (CMAKE_CXX_FLAGS "-std=c++20 ${CMAKE_CXX_FLAGS}")
			if (ENABLE_CXX)
				set(ENABLE_CXX20 ON)
			endif ()
		endif ()
	else ()
		message(FATAL_ERROR "The compiler doesn't support CXX.")
	endif ()
endif ()

check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)

if (HAVE_TIME_H AND HAVE_SYS_TIME_H)
	set(TIME_WITH_SYS_TIME ON)
endif ()

check_function_exists(getpid HAVE_GETPID)
check_function_exists(syscall HAVE_GETTID_SYSCALL)
check_function_exists(isinf HAVE_ISINF)
check_function_exists(isnan HAVE_ISNAN)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(qsort_r HAVE_QSORT_R)
check_function_exists(time HAVE_TIME)
check_function_exists(copysign HAVE_COPYSIGN)

# check_type_size (int32_t SIZEOF_INT32_T)
check_type_size(uint32_t SIZEOF_UINT32_T)
set(HAVE_UINT32_T ${SIZEOF_UINT32_T})
check_type_size("unsigned int" SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
message(STATUS "   sizeof uint32_t      = ${SIZEOF_UINT32_T}")
message(STATUS "   sizeof unsigned int  = ${SIZEOF_UNSIGNED_INT}")
message(STATUS "   sizeof unsigned long = ${SIZEOF_UNSIGNED_LONG}")

check_library_exists("m" sqrt "" HAVE_LIBM)

if (HAVE_LIBM)
	set(M_LIBRARY m)
endif ()

check_include_files("stdio.h;string.h" HAVE_STDIO_AND_STRING_H)
check_include_files("stdio.h;unistd.h" HAVE_EX_UNISTD_H)
