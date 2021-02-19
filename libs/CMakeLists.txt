cmake_minimum_required(VERSION 3.5)

#add_subdirectory(cmdr11)

#message("  -> CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
cmake_subdir_list(SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}")
foreach (subdir ${SUBDIRS})
    message("")
    message("!! entering subdir: ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    add_subdirectory(${subdir})
endforeach ()
#MESSAGE()

