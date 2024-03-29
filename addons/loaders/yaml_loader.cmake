# message(STATUS "1. CI_RUNNING = $ENV{CI_RUNNING}")
# if (!$ENV{CI_RUNNING})
#
# include(ExternalProject)
# ExternalProject_Add(third-yaml-cpp
# GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
# GIT_TAG origin/master   # yaml-cpp-0.6.3
# GIT_SHALLOW 1
# # STEP_TARGETS build
# # SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/yaml-cpp-src"
# # BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp-build"
# CMAKE_ARGS
# -DYAML_CPP_BUILD_TESTS=OFF
# --no-warn-unused-cli
# -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
# # -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
# # -DCMAKE_INSTALL_PREFIX:PATH=${EXECUTABLE_OUTPUT_PATH}
# BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting $<CONFIG> build"
# # COMMAND ${CMAKE_COMMAND} -E sudo "chmod a+w /usr/local/lib"
# COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
# COMMAND ${CMAKE_COMMAND} -E echo "$<CONFIG> build complete"
# )
#
# endif ()
#
#
# macro(add_yaml_loader target)
# if (${MACOS})
# target_include_directories(${target} PRIVATE
# $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
# $<INSTALL_INTERFACE:include>
# /usr/local/include
# )
# target_link_directories(${target} PRIVATE
# /usr/local/lib
# # ${CMAKE_CURRENT_BINARY_DIR}/third-yaml-cpp-prefix/src/third-yaml-cpp-build
# )
# target_link_libraries(${target}
# PRIVATE
# yaml-cpp
# )
# endif ()
#
# message(STATUS "2. CI_RUNNING = $ENV{CI_RUNNING}")
# if ($ENV{CI_RUNNING})
# target_include_directories(${target} PRIVATE
# $<BUILD_INTERFACE:${CMAKE_GENERATED_DIR}>
# $<INSTALL_INTERFACE:include>
# /usr/local/include
# )
# target_link_directories(${target} PRIVATE
# /usr/local/lib
# # ${CMAKE_CURRENT_BINARY_DIR}/third-yaml-cpp-prefix/src/third-yaml-cpp-build
# )
# target_link_libraries(${target}
# PRIVATE
# yaml-cpp
# )
# else ()
# message(STATUS "yaml-cpp: add_dependencies")
# add_dependencies(${target} third-yaml-cpp)
# endif ()
# endmacro()

# message(STATUS "yaml_loader: defined macro add_cmdr_cxx_to")
macro(add_yaml_loader_to target)
	find_package(yaml-cpp CONFIG REQUIRED)

	# ${YAML_CPP_INCLUDE_DIR}
	# ${YAML_CPP_LIBRARIES}
	if(NOT ${yaml-cppFOUND})
		message(STATUS "yaml_loader: yaml-cpp not found, try to pick one...")

		set(YAML_PP_TGT_NAME "third-yaml-cpp")
		include(ExternalProject)
		ExternalProject_Add(${YAML_PP_TGT_NAME}
			GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
			GIT_TAG yaml-cpp-0.6.3
			GIT_SHALLOW 1
			GIT_PROGRESS ON

			# STEP_TARGETS build
			# SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/cmdr-cxx-src"
			# BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmdr-cxx-build"
			CMAKE_ARGS
			-DYAML_CPP_BUILD_TESTS=OFF
			--no-warn-unused-cli
			-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}

			# -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
			# -DCMAKE_INSTALL_PREFIX:PATH=${EXECUTABLE_OUTPUT_PATH}
			BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Starting $<CONFIG> build"
			BUILD_COMMAND ${CMAKE_COMMAND} -E sudo "chmod a+w /usr/local/lib /usr/local/include"
			COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
			COMMAND ${CMAKE_COMMAND} -E echo "$<CONFIG> build complete"
		)

		# set(CMDR11_FOUND ON)
		message(STATUS "yaml_loader: add ${YAML_PP_TGT_NAME} module to '${target}' from building dir.")

		message(STATUS "yaml_loader: add_dependencies")
		add_dependencies(${target} ${YAML_PP_TGT_NAME})
		target_link_libraries(${target}
			PRIVATE
			yaml-cpp
		)

	# endif ()
	else()
		message(STATUS "yaml_loader: cmake package found at ${YAML_CPP_INCLUDE_DIR}, LIBS: ${YAML_CPP_LIBRARIES}")
		message(STATUS "yaml_loader: add ${YAML_PP_TGT_NAME} module to '${target}' from CMake Modules registry.")
		target_link_libraries(${target}
			PRIVATE
			yaml-cpp
		)
	endif()

	if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin" OR APPLE)
		message(STATUS "yaml_loader: for '${target}', added /usr/local/{include,lib}.")
		target_include_directories(${target} SYSTEM PRIVATE
			/usr/local/include
		)
		target_link_directories(${target} PRIVATE
			/usr/local/lib

			# ${CMAKE_CURRENT_BINARY_DIR}/${YAML_PP_TGT_NAME}-prefix/src/${YAML_PP_TGT_NAME}-build
		)

		# target_link_libraries(${target}
		# PRIVATE
		# cmdr11::cmdr11
		# )
	endif()
endmacro()
