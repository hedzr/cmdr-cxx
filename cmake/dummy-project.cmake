

#
# CPU bits
#

project(dummy)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
    set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
    message(STATUS "x86_64 ...")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
    set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
    message(STATUS "x86 (32bit) ...")
else ()
    # message(STATUS "Unknown CMAKE_SIZEOF_VOID_P = '${CMAKE_SIZEOF_VOID_P}' ..?")
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(amd64)|(AMD64)|(x86_64)|(arm64)|(aarch64)")
        set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
        #set(CPU_ARCH_NAME "x86_64" CACHE STRING "ARCH x86_64" FORCE)
        set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
        message(STATUS "x86_64 ...")
    else ()
        set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
        set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
        message(STATUS "x86 (32bit) // '${CMAKE_HOST_SYSTEM_PROCESSOR}' ...")
    endif ()
endif ()
