
# https://cmake.org/cmake/help/latest/module/WriteCompilerDetectionHeader.html
# include(WriteCompilerDetectionHeader)
# https://cmake.org/cmake/help/v3.14/manual/cmake-compile-features.7.html#manual:cmake-compile-features(7)
set(WriterCompilerDetectionHeaderFound NOTFOUND)
# This module is only available with CMake >=3.1, so check whether it could be found
# BUT in CMake 3.1 this module doesn't recognize AppleClang as compiler, so just use it as of CMake 3.2
if (${CMAKE_VERSION} VERSION_GREATER "3.2")
  include(WriteCompilerDetectionHeader OPTIONAL RESULT_VARIABLE WriterCompilerDetectionHeaderFound)
endif ()
if (WriterCompilerDetectionHeaderFound)
  write_compiler_detection_header(
          FILE ${CMAKE_GENERATED_DIR}/the-compiler.h
          PREFIX The
          COMPILERS GNU Clang MSVC Intel AppleClang     #  SunPro
          FEATURES
          cxx_variadic_templates cxx_final cxx_override
          cxx_static_assert cxx_constexpr cxx_extern_templates
          cxx_noexcept cxx_thread_local
  )
endif ()

#
# CMAKE_POSITION_INDEPENDENT_CODE
#
include(TestBigEndian)
if (NOT WIN32)
  TEST_BIG_ENDIAN(_bigendian)
  if ((CMAKE_SIZEOF_VOID_P GREATER 4) OR (_bigendian))
    message(
            STATUS ">> Setting PIC (${CMAKE_CXX_COMPILE_OPTIONS_PIC}) for machine ${CMAKE_HOST_SYSTEM_PROCESSOR}"
    )
    set(CMAKE_POSITION_INDEPENDENT_CODE 1)
  endif ()
endif ()

#
# CPU bits
#
#if (CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
#  set(CPU_ARCH x64)
#else ()
#  set(CPU_ARCH x86)
#endif ()
#mark_as_advanced(CPU_ARCH)

