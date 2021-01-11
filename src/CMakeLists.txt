
#message(STATUS "This is lexer-BINARY dir " ${lexer_BINARY_DIR})
#message(STATUS "This is lexer-SOURCE dir " ${lexer_SOURCE_DIR})
#message(STATUS "This is lexer-INCLUDE dir " ${CMAKE_CURRENT_SOURCE_DIR}/include)
#message(STATUS "This is PROJECT_SOURCE_DIR dir " ${PROJECT_SOURCE_DIR})

#debug_print_project_title()
##debug_print_value(LEXER_INCS)

#debug_print_hbar()
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()

#message("  -> CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
cmake_subdir_list(SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}")
foreach (subdir ${SUBDIRS})
    message("")
    message("!! entering subdir: ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    add_subdirectory(${subdir})
endforeach ()
#MESSAGE()



