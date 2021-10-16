project(examples
        VERSION ${VERSION}
        DESCRIPTION "examples - example-apps for cmdr cxx11 library"
        LANGUAGES CXX)


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

find_package(Threads)

function(define_example_program name)
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
                          Threads::Threads
                          # Catch2::Catch2
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
        target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                               -pedantic -Wall -Wextra -Wshadow -Werror
                               )
        if (DEBUG)
            target_compile_options(${PROJECT_NAME}-${name} PRIVATE
                                   -fsanitize=address
                                   # address(AddressSanitizer), leak(LeakSanitizer), thread(ThreadSanitizer), undefined(UndefinedBehaviorSanitizer), memory(MemorySanitizer) 
                                   # [additional_options]: -fno-omit-frame-pointer, fsanitize-recover/fno-sanitize-recover, -fsanitize-blacklist, etc.
                                   # [-g] [-OX]
                                   )
            target_link_options(${PROJECT_NAME}-${name} PRIVATE -fsanitize=address)
        endif ()
    endif ()
endfunction()


define_example_program(app1 app1.cc second_src.cc)
define_example_program(app2 app2.cc second_src.cc)




#
#if (MSVC)
#    # target_compile_options(test-sso-1 PRIVATE /W4 /WX /utf-8)
#    target_compile_options(test-flags-c2 PRIVATE /bigobj)
#else ()
#    # target_compile_options(test-sso-1 PRIVATE -Werror=sized-deallocation)
#endif ()

#
# For test-app2-c1, loading the dependency to yaml-cpp
#
include(loaders/yaml_loader)
add_yaml_loader_to(examples-app1)
#target_compile_definitions(examples-app1 PRIVATE CMDR_ENABLE_VERBOSE_LOG=0)

#target_compile_definitions(examples PRIVATE CMDR_ENABLE_VERBOSE_LOG=0)

#if (BUILD_TESTING)
#    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
#endif ()


add_subdirectory(more)


debug_print_value(ARCHIVE_NAME)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
debug_print_value(CMAKE_SOURCE_DIR)
message(STATUS "end of tests")