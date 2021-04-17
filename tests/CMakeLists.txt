project(test
        VERSION ${VERSION}
        DESCRIPTION "tests - test apps for cmdr cxx11 library"
        LANGUAGES C CXX)


####################################
#dump_list(INCLUDE_DIRECTORIES)
#debug_print_list_value(INCLUDE_DIRECTORIES)

debug_print_hbar()
debug("  tests ")
message("!! entering subdir: ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()


set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

if (ENABLE_TESTS)
    include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif ()


## so_stuff: just a tester for ARGN
#function(do_stuff arg1 arg2)
#    foreach (f ${ARGN})
#        # do something interesting with file 'f'
#        message("f='${f}' arg1='${arg1}' arg2='${arg2}'")
#    endforeach ()
#    message("f='${f}' arg1='${arg1}' arg2='${arg2}' ARGV2='${ARGV2}'")
#endfunction()
#do_stuff(A B C D E)

function(define_test_program name)
    #set(src_list )
    foreach (f ${ARGN})
        list(APPEND src_list ${f})
    endforeach ()

    add_executable(${PROJECT_NAME}-${name} ${src_list})
    #target_compile_features(${PROJECT_NAME}-${name} PRIVATE cxx_std_11)
    target_include_directories(${PROJECT_NAME}-${name} PRIVATE
                               $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                               ${CMAKE_SOURCE_DIR}
                               )
    target_link_libraries(${PROJECT_NAME}-${name}
                          PRIVATE
                          libs::cmdr11
                          Catch2::Catch2
                          # fmt::fmt-header-only
                          )
    #    set_target_properties(${PROJECT_NAME}-${name} PROPERTIES
    #            CXX_STANDARD 17
    #            CXX_STANDARD_REQUIRED ON
    #            CXX_EXTENSIONS OFF
    #            )
    if (MSVC)
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE /W4 /WX /utf-8)
    else ()
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE -pedantic -Wall -Wextra -Wshadow -Werror)
    endif ()


    if (${ENABLE_AUTOMATE_TESTS})
        get_property(tmp GLOBAL PROPERTY UNIT_TEST_TARGETS)
        set(tmp ${UNIT_TEST_TARGETS} ${PROJECT_NAME}-${name})
        set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "${tmp}")
        message(">> add_test(${PROJECT_NAME}-${name}) ...")
    endif ()

    if (ANDROID)
        add_test(NAME ${PROJECT_NAME}-${name}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_COMMAND} "-DANDROID_NDK=${ANDROID_NDK}"
                 "-DTEST_RESOURCES_DIR=${CMAKE_SOURCE_DIR}"
                 "-DTEST_RESOURCES=tests/data;tests/file_data.txt;Makefile"
                 "-DUNITTEST=${PROJECT_NAME}-${name}"
                 -P ${CMAKE_CURRENT_SOURCE_DIR}/ExecuteOnAndroid.cmake)
    else ()
        add_test(NAME ${PROJECT_NAME}-${name}
                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                 COMMAND $<TARGET_FILE:${PROJECT_NAME}-${name}>)
    endif ()
endfunction()


define_test_program(sso-1 sso_test_1.cc)
define_test_program(sso-2 sso_test_2.cc)
define_test_program(any-1 any_1.cc)
define_test_program(visit-any
                    visit_any.cc
                    second_src.cc)
define_test_program(chrono chrono_test.cc)
define_test_program(chrono-literals chrono_literals_test.cc)
define_test_program(inplace-constructions inplace_constructions.cc)
define_test_program(valarray-test valarray_test.cc)
define_test_program(complex-test complex_test.cc)
define_test_program(mem-singleton-1 mem_test_for_singleton.cc)

define_test_program(priority-queue priority_queue.cc)

define_test_program(store-c2 store_c2.cc)
define_test_program(flags-c2 flags_c2.cc second_src.cc)
define_test_program(jaro-c2 jaro_c2.cc)


# app2 c1
define_test_program(app2-c1 app1.cc second_src.cc)


# app2 c2
define_test_program(app2-c2 app2.cc second_src.cc)


#
if (MSVC)
    # target_compile_options(test-sso-1 PRIVATE /W4 /WX /utf-8)
    target_compile_options(test-flags-c2 PRIVATE /bigobj)
else ()
    # target_compile_options(test-sso-1 PRIVATE -Werror=sized-deallocation)
endif ()

#
# For test-app-c1, loading the dependency to yaml-cpp
#
include(loaders/yaml_loader)
add_yaml_loader_to(test-app2-c1)
target_compile_definitions(test-app2-c1 PRIVATE
                           CMDR_ENABLE_VERBOSE_LOG=1)

target_compile_definitions(app1 PRIVATE
                           CMDR_ENABLE_VERBOSE_LOG=1)

#if (BUILD_TESTING)
#    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
#endif ()
debug_print_value(ARCHIVE_NAME)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
debug_print_value(CMAKE_SOURCE_DIR)
message(STATUS "end of tests")