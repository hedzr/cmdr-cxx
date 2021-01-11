
# FORCE gcc
#if (MACOS)
#    set(CMAKE_C_COMPILER "gcc-9")
#    set(CMAKE_CXX_COMPILER "g++-9")
#endif()
#message("CMAKE_C_COMPILER:        ${CMAKE_C_COMPILER}")
#message("CMAKE_CXX_COMPILER:      ${CMAKE_CXX_COMPILER}")


#set(default_build_type "Release")
#if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
#  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
#  # Set the possible values of build type for cmake-gui
#  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
#endif()

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

if (DEBUG AND MINGW)
    add_link_options(--allow-multiple-definition)
endif ()

if (DEBUG)
    add_compile_options(-D_DEBUG -DDEBUG=1)
else ()
    add_compile_options(-DNDEBUG=1)
endif ()

#
#
