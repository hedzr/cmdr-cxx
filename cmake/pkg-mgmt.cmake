# ######################################################
option(USE_CONAN "enable package manager: conan" OFF)
option(USE_VCPKG "enable package manager: vcpkg" OFF)

macro (load_package_manager)
	# ######################################################
	if (USE_CONAN)
		# run this : cd build && conan install ..
		# or: conan install . --install-folder build
		find_program(CONAN_CMD conan PATHS
				"/usr/local/bin"
				"$ENV{HOME}/.conan/bin"
				"$ENV{HOME}/.local/bin"
		)
		
		if (NOT CONAN_CMD)
			message(FATAL_ERROR "Conan: not found!")
			set(USE_CONAN OFF FORCE)
		else ()
			message(STATUS "Conan: FOUND at ${CONAN_CMD}")
		endif ()
		
		if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
			exec_program(${CONAN_CMD} ${CMAKE_CURRENT_SOURCE_DIR} ARGS "install . --install-folder ${CMAKE_BINARY_DIR}")
		endif ()
		
		include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
		conan_basic_setup()
		
		output_conan_include_dirs()
	endif ()
	
	# ######################################################
	if (USE_VCPKG)
		if (NOT DEFINED ENV{VCPKG_ROOT})
			find_program(VCPKG_CMD vcpkg PATHS
					/usr/local/opt/vcpkg/bin
			)
			
			if (VCPKG_CMD_FOUND)
				get_filename_component(VCPKG_ROOT $VCPKG_CMD DIRECTORY)
			endif ()
		else ()
			set(VCPKG_ROOT $ENV{VCPKG_ROOT})
		endif ()
		
		if (DEFINED ENV{VCPKG_TARGET_TRIPLET})
			set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_TARGET_TRIPLET})
		endif ()
		
		if (DEFINED VCPKG_ROOT)
			set(VCPKG_FEATURE_FLAGS "versions")
			set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
					CACHE STRING "Vcpkg toolchain file")
			message(STATUS ">>> Using vcpkg with VCPKG_ROOT = ${VCPKG_ROOT}")
			message(STATUS "    CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
			message(STATUS "    VCPKG_TARGET_TRIPLET = ${VCPKG_TARGET_TRIPLET}") # =x64-windows, ...
		else ()
			message(STATUS ">>> vcpkg ignored - you may install it at first")
		endif ()
	endif ()
endmacro ()