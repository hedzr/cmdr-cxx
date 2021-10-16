if (EXISTS ${CMAKE_SOURCE_DIR}/.version.cmake)
    message("   version decl file found: ${CMAKE_SOURCE_DIR}/.version.cmake")
    include(.version)
    #elseif (${EXIST})
else ()
    message("   version decl file ignored")
    set(VERSION 0.1.0.1)
endif ()