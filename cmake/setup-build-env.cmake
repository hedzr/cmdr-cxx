
# FORCE gcc
#if (${MACOS})
#    set(CMAKE_C_COMPILER "gcc-9")
#    set(CMAKE_CXX_COMPILER "g++-9")
#endif()
#message("CMAKE_C_COMPILER:        ${CMAKE_C_COMPILER}")
#message("CMAKE_CXX_COMPILER:      ${CMAKE_CXX_COMPILER}")


if (${MSVC})
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX
        /D_CRT_SECURE_NO_WARNINGS
        )
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror
        -Wno-unknown-pragmas
        # -Weffc++     # warning: ‘operator=’ should return a reference to ‘*this’
        )
endif ()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    # gcc-9: std::is_constructible
    add_compile_options(-D_LIBCPP_TESTING_FALLBACK_IS_CONSTRUCTIBLE)
    message(">  GCC ${CMAKE_CXX_COMPILER_VERSION} FOUND")
else ()
    message(">  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} FOUND")
endif ()

if ((${USE_DEBUG}==ON) AND (${MINGW}==ON))
    add_link_options(--allow-multiple-definition)
endif ()

if (${USE_DEBUG})
    add_compile_options(-D_DEBUG -DDEBUG=1)
else ()
    add_compile_options(-DNDEBUG=1)
endif ()

if (${USE_DEBUG})
    #add_compile_options(-DUSE_DEBUG_MALLOC=${USE_DEBUG_MALLOC})

    # enable google sanitizer
    # add_compile_options(-fsanitize=[sanitizer_name] [additional_options] [-g] [-OX])
endif ()

#
#
