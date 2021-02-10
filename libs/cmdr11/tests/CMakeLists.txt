cmake_minimum_required(VERSION 3.5)
project(cmdr11_tests)

add_executable(${PROJECT_NAME}
               main.cxx
               test_store.cc
               )
target_include_directories(${PROJECT_NAME} PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
                           )
target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      libs::cmdr11)

#target_link_libraries(${PROJECT_NAME}
#        PRIVATE
#        libs::sm-lib
#        )
