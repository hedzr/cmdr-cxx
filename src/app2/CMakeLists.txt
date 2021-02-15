project(app2
        VERSION ${VERSION}
        DESCRIPTION "app1 - test app for cmdr cxx11 library"
        LANGUAGES CXX)


####################################
#dump_list(INCLUDE_DIRECTORIES)
#debug_print_list_value(INCLUDE_DIRECTORIES)

#debug_print_hbar()
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
add_executable(${PROJECT_NAME} main.cc)
#cxx_set_ipo(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_include_directories(${PROJECT_NAME} PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                           )
target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      libs::cmdr11)
set_target_properties(${PROJECT_NAME} PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS ON
                      )

if (BUILD_TESTING)
    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
endif ()
debug_print_value(ARCHIVE_NAME)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
