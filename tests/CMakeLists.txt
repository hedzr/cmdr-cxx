project(test
        VERSION ${CMAKE_PROJECT_VERSION}
        DESCRIPTION "tests - test apps for cmdr cxx11 library"
        LANGUAGES C CXX)


####################################
#dump_list(INCLUDE_DIRECTORIES)
#debug_print_list_value(INCLUDE_DIRECTORIES)

#debug_print_hbar()
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()


set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

# so_stuff: just a tester for ARGN
function(do_stuff arg1 arg2)
    foreach (f ${ARGN})
        # do something interesting with file 'f'
        message("f='${f}' arg1='${arg1}' arg2='${arg2}'")
    endforeach ()
    message("f='${f}' arg1='${arg1}' arg2='${arg2}' ARGV2='${ARGV2}'")
endfunction()
do_stuff(A B C D E)

function(define_test_program name)
    #set(src_list )
    foreach (f ${ARGN})
        list(APPEND src_list ${f})
    endforeach ()
    add_executable(${PROJECT_NAME}-${name} ${src_list})
    #target_compile_features(${PROJECT_NAME}-${name} PRIVATE cxx_std_11)
    target_include_directories(${PROJECT_NAME}-${name} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
            )
    target_link_libraries(${PROJECT_NAME}-${name}
            PRIVATE
            libs::cmdr11)
endfunction()


define_test_program(sso-1 sso_test_1.cc)
define_test_program(sso-2 sso_test_2.cc)
define_test_program(visit-any
        visit_any.cc
        second_src.cc)
define_test_program(chrono chrono_test.cc)
define_test_program(chrono-literals chrono_literals_test.cc)
define_test_program(inplace-constructions inplace_constructions.cc)


#if (BUILD_TESTING)
#    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
#endif ()
debug_print_value(ARCHIVE_NAME)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
