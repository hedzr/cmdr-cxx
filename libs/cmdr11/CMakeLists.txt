project(cmdr11
        VERSION ${CMAKE_PROJECT_VERSION}
        DESCRIPTION "libcmdr11 - cmdr cxx11 library"
        LANGUAGES CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
#if (POLICY CMP0069)
#    cmake_policy(SET CMP0069 NEW)
#endif ()
if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()

# option(BUILD_TESTING "Build the testing tree." ON)
#
# To enable Nightly Target and More, uncomment and include CTest script from CMake bundle.
# include(CTest)


#find_package(ZLIB REQUIRED)


#debug_print_hbar()
#debug_dump_cmake_environments()
#debug_print_hbar()
#debug_dump_cmake_variables()
#debug_print_hbar_long()


add_library(${PROJECT_NAME} INTERFACE)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
#cxx_set_ipo(${PROJECT_NAME})
#set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "libs/${PROJECT_NAME}")


target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)
target_sources(${PROJECT_NAME} INTERFACE include/cmdr11/cmdr11.hh)
#target_sources(${PROJECT_NAME} PRIVATE src/private.cc)
#set_target_properties(${PROJECT_NAME} PROPERTIES
#        COMPILE_FEATURES cxx_std_14
#        SOURCES src/${PROJECT_NAME}.cc
#        )
target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        #PRIVATE
        #${ZLIB_INCLUDE_DIRS}
        )
#target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})
#target_link_libraries(${PROJECT_NAME}
#        PRIVATE
#        libs::sm-lib)

if (BUILD_TESTING)
    message(STATUS trying add test with name ${PROJECT_NAME}_tests)
    add_unit_test(${PROJECT_NAME} tests ${PROJECT_NAME}_tests)
endif ()
debug_print_value(ARCHIVE_NAME)
debug_print_value(PROJECT_ARCHIVE_NAME)
debug_print_value(BUILD_TESTING)
debug_print_value(CMAKE_CURRENT_SOURCE_DIR)
debug_print_value(PROJECT_SOURCE_DIR)

#debug_dump_target_inc_dirs_interface_only(${PROJECT_NAME})
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "    = '${dir}'")
endforeach ()

get_directory_property(inc_dirs INCLUDE_DIRECTORIES)
list(TRANSFORM inc_dirs PREPEND "-I ")
list(JOIN inc_dirs " " INCLUDE_STRING)
debug_print_value(inc_dirs)