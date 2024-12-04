# https://github.com/Kitware/CMake/blob/master/Modules/CMakeDetermineSystem.cmake
#
# Expected uname -s output:
#
# AIX                           AIX
# BSD/OS                        BSD/OS
# FreeBSD                       FreeBSD
# HP-UX                         HP-UX
# Linux                         Linux
# GNU/kFreeBSD                  GNU/kFreeBSD
# NetBSD                        NetBSD
# OpenBSD                       OpenBSD
# OFS/1 (Digital Unix)          OSF1
# SCO OpenServer 5              SCO_SV
# SCO UnixWare 7                UnixWare
# SCO UnixWare (pre release 7)  UNIX_SV
# SCO XENIX                     Xenix
# Solaris                       SunOS
# SunOS                         SunOS
# Tru64                         Tru64
# Ultrix                        ULTRIX
# cygwin                        CYGWIN_NT-5.1
# MacOSX                        Darwin

# for MacOS X or iOS, watchOS, tvOS (since 3.10.3)
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin" OR APPLE)
	# MESSAGE("** Darwin detected.")
	SET(MACOSX TRUE)
	SET(MACOS TRUE)
	SET(macOS TRUE)
	SET(DARWIN TRUE)
	SET(MAC TRUE)
	SET(Mac TRUE)
	mark_as_advanced(MACOSX MACOS macOS DARWIN MAC Mac)
endif ()

if (UNIX AND NOT macOS)
	# for Linux, BSD, Solaris, Minix
	if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
		set(LINUX TRUE)
		set(Linux TRUE)
	elseif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "^GNU/kFreeBSD|NetBSD|OpenBSD")
		set(BSD TRUE)
	elseif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Minix")
		set(Minix TRUE)
		set(MINIX TRUE)
	endif ()
	
	mark_as_advanced(LINUX Linux BSD Minix MINUX)
endif ()

if (WIN32)
	# do something
endif (WIN32)

if (MSVC OR MSYS OR MINGW)
	# for detecting Windows compilers
endif ()

if (LINUX)
	# file(READ "/etc/issue" ETC_ISSUE)
	# string(REGEX MATCH "Debian|Ubuntu" DIST ${ETC_ISSUE})
	file(STRINGS /etc/os-release distro REGEX "^NAME=")
	string(REGEX REPLACE "NAME=\"(.*)\"" "\\1" distro "${distro}")
	
	# if (string (FIND distro "NAME=" vd))
	string(REGEX REPLACE "NAME=(.*)" "\\1" distro "${distro}")
	
	# endif ()
	string(REGEX REPLACE "(.+) Linux" "\\1" distro "${distro}")
	file(STRINGS /etc/os-release distro_id REGEX "^ID=")
	string(REGEX REPLACE "ID=\"(.*)\"" "\\1" distro_id "${distro_id}")
	string(REGEX REPLACE "ID=(.*)" "\\1" distro_id "${distro_id}")
	file(STRINGS /etc/os-release disversion REGEX "^VERSION_ID=")
	string(REGEX REPLACE "VERSION_ID=\"(.*)\"" "\\1" disversion "${disversion}")
	string(REGEX REPLACE "VERSION_ID=(.*)" "\\1" disversion "${disversion}")
	message(">>> found ${distro}.${disversion} id=${distro_id}")
	set(DISTRO_NAME ${distro})
	set(DISTRO_ID ${distro_id})
	set(DISTRO_VERSION ${disversion})
	mark_as_advanced(DISTRO_NAME DISTRO_ID DISTRO_VERSION)
endif ()

message(STATUS ">>> CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS ">>> DISTRO_NAME = ${DISTRO_NAME}")

if (LINUX)
	message(STATUS ">>> Linux" " (Distro: ${DISTRO_NAME})" " FOUND")
elseif (MAC OR APPLE)
	message(STATUS ">>> macOS" " FOUND")
elseif (UNIX)
	message(STATUS ">>> Unix (BSD+,Unix+)" " FOUND")
elseif (MSVC)
	message(STATUS ">>> MSVC" " FOUND")
elseif (WIN32)
	message(STATUS ">>> Win32" " FOUND")
else ()
	message(STATUS ">>> Unknown System" " '${CMAKE_SYSTEM}' FOUND")
endif ()

if (MINGW)
	message(STATUS ">>> MinGW / MSYS2" " FOUND")
elseif (MSYS)
	message(STATUS ">>> MSYS" " FOUND (${CMAKE_SYSTEM})")
endif ()

message(STATUS "BUILD_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")

if (CYGWIN OR WIN32 OR APPLE)
	set(COMPILING_FS_CASE_INSENSITIVE TRUE)
else ()
	set(COMPILING_FS_CASE_INSENSITIVE FALSE)
endif ()

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ")
message(STATUS ">>> BUILD_TIMESTAMP = ${BUILD_TIMESTAMP}")

#
#
#
