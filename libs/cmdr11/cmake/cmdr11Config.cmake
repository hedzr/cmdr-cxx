## Get the directory path of the <target>.cmake file
## get_filename_component(SM_LIB_CMAKE_DIR "cmdr11Config.cmake" DIRECTORY)

## Add the dependencies of our library
include(CMakeFindDependencyMacro)
#find_dependency(ZLIB REQUIRED)
#find_dependency(fmt REQUIRED)

## Import the targets
if (NOT TARGET cmdr11::cmdr11)
  #include("${SM_LIB_CMAKE_DIR}/cmdr11Targets.cmake")
  include("${CMAKE_CURRENT_LIST_DIR}/cmdr11Targets.cmake")
endif ()