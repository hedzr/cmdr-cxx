

include(ExternalProject)
ExternalProject_Add(third-yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master   # yaml-cpp-0.6.3
        # STEP_TARGETS build
        # SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/yaml-cpp-src"
        # BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp-build"
        CMAKE_ARGS
        -DYAML_CPP_BUILD_TESTS=OFF
        --no-warn-unused-cli
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        # -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
        BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting $<CONFIG> build"
        COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
        COMMAND ${CMAKE_COMMAND} -E echo "$<CONFIG> build complete"
        )

macro(add_yaml_loader target)
  if (MACOS)
    target_include_directories(${target} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
            $<INSTALL_INTERFACE:include>
            /usr/local/include
            )
    target_link_directories(${target} PRIVATE
            /usr/local/lib
            # ${CMAKE_CURRENT_BINARY_DIR}/third-yaml-cpp-prefix/src/third-yaml-cpp-build
            )
    target_link_libraries(${target}
            PRIVATE
            yaml-cpp
            )
  endif ()
  add_dependencies(${target} third-yaml-cpp)
endmacro()

