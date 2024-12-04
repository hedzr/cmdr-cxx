# if (EXISTS ${CMAKE_SOURCE_DIR}/.version.cmake)
# include(.version)
# message(STATUS ">>> version decl file found: ${CMAKE_SOURCE_DIR}/.version.cmake (V${VERSION})")
# #elseif (EXIST)
# else ()
# message(STATUS ">>> version decl file ignored")
# set(VERSION 0.1.0.1)
# endif ()
set(VERSION 0.1.0.1)
if (EXISTS ${CMAKE_SOURCE_DIR}/.build-serial)
    message(STATUS ".build-serial found")
else()
	file(WRITE ${CMAKE_SOURCE_DIR}/.build-serial "1")
endif()
safe_include(.version.cmake "version declaration file")
safe_include(.version.local.cmake "local version declaration file")