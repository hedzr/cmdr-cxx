
# https://github.com/Kitware/CMake/blob/master/Modules/CMakeDetermineSystem.cmake
#
#  Expected uname -s output:
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
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR APPLE)
    #MESSAGE("** Darwin detected.")
    SET(MACOSX TRUE)
    SET(MACOS TRUE)
    SET(macOS TRUE)
    SET(DARWIN TRUE)
    SET(MAC TRUE)
    SET(Mac TRUE)
    mark_as_advanced(MACOSX MACOS macOS DARWIN MAC Mac)
endif ()

if (UNIX AND NOT MACOS)
    # for Linux, BSD, Solaris, Minix
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LINUX TRUE)
        set(Linux TRUE)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "^GNU/kFreeBSD|NetBSD|OpenBSD")
        set(BSD TRUE)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Minix")
        set(Minix TRUE)
        set(MINIX TRUE)
    endif ()
    mark_as_advanced(LINUX Linux BSD Minix MINUX)
endif ()

if (WIN32)
    #do something
endif (WIN32)

if (MSVC OR MSYS OR MINGW)
    # for detecting Windows compilers
endif ()

if (LINUX)
    message(STATUS ">>> Linux" " FOUND")
elseif (MAC)
    message(STATUS ">>> macOS" " FOUND")
elseif (UNIX)
    message(STATUS ">>> Unix (BSD+,Unix+)" " FOUND")
elseif (MSVC)
    message(STATUS ">>> MSVC" " FOUND")
elseif (WIN32)
    message(STATUS ">>> Win32" " FOUND")
else ()
    message(STATUS ">>> Unknown" " FOUND")
endif ()
if (MINGW)
    message(STATUS ">>> MinGW" " FOUND")
endif ()

#
#
#



