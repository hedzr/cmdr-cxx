
include(CMakePrintHelpers)
include(FeatureSummary)

function (debug_dump_target_inc_dirs_all target)
	get_property(dirs1 TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
	get_property(dirs2 TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
	list(APPEND union_list ${dirs1} ${dirs2} "SAFETY")
	list(REMOVE_DUPLICATES union_list)
	list(REMOVE_ITEM union_list "SAFETY")
	message(STATUS "- List of INC DIRS -------------")
	
	foreach (dir ${${union_list}})
		message(STATUS "          - ${dir}")
	endforeach (dir)
endfunction ()

# NOTE: debug_dump_target_inc_dirs can't work for a interface library.
function (debug_dump_target_inc_dirs target)
	get_property(dirs1 TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
	get_property(dirs2 TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
	list(APPEND Tprivate ${dirs1} "SAFETY")
	list(REMOVE_ITEM Tprivate ${dirs2} "SAFETY")
	list(APPEND Tpublic ${dirs1} "SAFETY")
	list(REMOVE_ITEM Tpublic ${Tprivate} "SAFETY")
	list(APPEND Tinterface ${dirs2} "SAFETY")
	list(REMOVE_ITEM Tinterface ${dirs1} "SAFETY")
	message(STATUS "- List of INC DIRS ------------- PUBLIC")
	
	foreach (dir ${${Tpublic}})
		message("          - ${dir}")
	endforeach (dir)
	
	message(STATUS "- List of INC DIRS ------------- PRIVATE")
	
	foreach (dir ${${Tprivate}})
		message("          - ${dir}")
	endforeach (dir)
	
	message(STATUS "- List of INC DIRS ------------- INTERFACE")
	
	foreach (dir ${${Tinterface}})
		message("          - ${dir}")
	endforeach (dir)
endfunction ()

# function(debug_dump_target_inc_dirs_interface_only target)
# get_property(dirs2 TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
# message(STATUS "- List of INC DIRS ------------- INTERFACE")
# foreach (dir ${${dirs2}})
# message("          - ${dir}")
# endforeach (dir)
# endfunction()
macro (debug_print_hbar)
	message("|-----------------------------------")
endmacro ()

macro (debug_print_hbar_long)
	message("|------------------------------------------------------------------------")
endmacro ()

macro (debug_print_project_title)
	message("|------------------------------------------------------------------------")
	message("| Making current project(${PROJECT_NAME}) for archive ${ARCHIVE_NAME} ...")
	message("|------------------------------------------------------------------------")
endmacro ()

macro (debug_print_project_vars)
	debug_print_project_title()
	message("  CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}")
	message("  CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
	message("  CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
	message("  CMAKE_MODULE_PATH:        " ${CMAKE_MODULE_PATH})
	message("  CMAKE_SYSTEM_NAME:        ${CMAKE_SYSTEM_NAME}")
	message("  CMAKE_SYSTEM_VERSION:     ${CMAKE_SYSTEM_VERSION}")
	message("  CMAKE_SYSTEM_PROCESSOR:   ${CMAKE_SYSTEM_PROCESSOR}")
	message("  CMAKE_C_COMPILER:         ${CMAKE_C_COMPILER}")
	message("  CMAKE_CXX_COMPILER:       ${CMAKE_CXX_COMPILER}")
	message("  CMAKE_BUILD_TYPE:         ${CMAKE_BUILD_TYPE}")
	
	# project info
	message("  PROJECT_VERSION:          ${PROJECT_VERSION}")
	message("  ARCHIVE_NAME:             ${ARCHIVE_NAME}")
	
	message(STATUS "  CXX_FLAGS                  = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})
	
	# message(STATUS "  PQXX: " ${Pqxx_INCLUDE_DIRS} "; version: " ${Pqxx_VERSION_STRING})
	# message(STATUS "  CMAKE_REQUIRED_DEFINITIONS = " ${CMAKE_REQUIRED_DEFINITIONS})
	message(STATUS "  COMPILE_DEFINITIONS        = " ${COMPILE_DEFINITIONS})
	
	# message(STATUS "  INCLUDE_DIRECTORIES        = ")
	# get_directory_property(dirs INCLUDE_DIRECTORIES)
	# debug_print_list_value(dirs)
	message(STATUS "  Target.INCLUDE_DIRECTORIES        = ")
	get_target_property(dirs ${PROJECT_NAME} INCLUDE_DIRECTORIES)
	debug_print_list_value(dirs)
	
	# foreach (dir ${dirs})
	# message(STATUS "                       dir = '${dir}'")
	# endforeach ()
endmacro ()

function (debug_dump_cmake_environments)
	execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endfunction ()

macro (debug_print_top_vars)
	# ----------------------------------------------------------------------
	# Information Summary
	# ----------------------------------------------------------------------
	# cmake info
	message("------------------------------------------------------------------------")
	message("CMAKE_SOURCE_DIR:        ${CMAKE_SOURCE_DIR}")
	message("CMAKE_BINARY_DIR:        ${CMAKE_BINARY_DIR}")
	
	# message("EXECUTABLE_OUTPUT_PATH:  ${EXECUTABLE_OUTPUT_PATH}")
	# message("LIBRARY_OUTPUT_PATH:     ${LIBRARY_OUTPUT_PATH}")
	message("CMAKE_SYSTEM_NAME+VER:   ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
	message("CMAKE_SYSTEM_PROCESSOR:  ${CMAKE_SYSTEM_PROCESSOR}")
	message("CMAKE_C_COMPILER:        ${CMAKE_C_COMPILER}")
	message("CMAKE_CXX_COMPILER:      ${CMAKE_CXX_COMPILER}")
	message("CMAKE_BUILD_TYPE:        ${CMAKE_BUILD_TYPE}")
	
	# project info
	message("-------------------------")
	message("PROJECT_VERSION:         ${PROJECT_VERSION}")
	message("ARCHIVE_NAME:            ${ARCHIVE_NAME}")
	message("-------------------------")
	message("EXECUTABLE_OUTPUT_PATH:  ${EXECUTABLE_OUTPUT_PATH}")
	message("LIBRARY_OUTPUT_PATH:     ${LIBRARY_OUTPUT_PATH}")
	
	# # dependency info
	# message("-------------------------")
	# message("SKIA:                    ${ENABLE_SKIA} -> ${USE_SKIA}")
	# message("CAIRO:                   ${ENABLE_CAIRO} -> ${USE_CAIRO}")
	# message("GLX:                     ${ENABLE_GLX} -> ${USE_GLX}")
	# message("EGL:                     ${ENABLE_EGL} -> ${USE_EGL}")
	# message("GLUT:                    ${ENABLE_GLUT} -> ${USE_GLUT}")
	# message("OPENGLES2:               ${OPENGLES2_FOUND}")
	# message("OPENGLES3:               ${OPENGLES3_FOUND}")
	# message("CAIROGLES:               ${ENABLE_CAIROGLES}")
	# message("CAIRO:                   ${CAIRO_INCLUDE_DIRS}")
	# message("SDL2:                    ${SDL2_INCLUDE_DIR} | libs: ${SDL2_LIBRARY}")
	# if (${ENABLE_SKIA})
	# message("SKIA:                    ${SKIA_INCLUDE_DIRS}")
	# endif ()
	# message("X11:                     ${X11_LIBRARIES}")
	
	# message("-------------------------")
	# message("LIBS:")
	# foreach (sm-lib ${C14R_LIBS})
	# message("                         ${sm-lib}")
	# endforeach (sm-lib)
	# message("-------------------------")
	# message("INCLUDES:")
	# foreach (inc ${CMAKE_INCLUDES})
	# message("                         ${inc}")
	# endforeach (inc)
	# message("INCLUDES (SYS):")
	# foreach (inc ${C14R_INCS_SYS})
	# message("                         ${inc}")
	# endforeach (inc)
	message("------------------------------------------------------------------------")
endmacro ()

# function(debug_dump_cmake_variables)
# get_cmake_property(_variableNames VARIABLES)
# list(SORT _variableNames)
# foreach (_variableName ${_variableNames})
# if (${ARGV0})
# unset(MATCHED)
# string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
# if (NOT (${MATCHED}))
# continue()
# endif ()
# endif ()
# message(STATUS "${_variableName}=${${_variableName}}")
# endforeach ()
# endfunction()

# help:
# debug_dump_cmake_variables("^Boost")
# debug_dump_cmake_variables()
function (debug_dump_cmake_variables)
	get_cmake_property(_variableNames VARIABLES)
	list(SORT _variableNames)
	
	foreach (_variableName ${_variableNames})
		if (${ARGV0})
			unset(MATCHED)
			
			# case sensitive match
			# string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
			#
			# case insenstitive match
			string(TOLOWER "${ARGV0}" ARGV0_lower)
			string(TOLOWER "${_variableName}" _variableName_lower)
			string(REGEX MATCH ${ARGV0_lower} MATCHED ${_variableName_lower})
			
			if (NOT MATCHED)
				continue()
			endif ()
		endif ()
		
		message(STATUS "  - ${_variableName} = ${${_variableName}}")
	endforeach ()
endfunction ()

macro (dump_feature_summary)
	if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND)
		feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)
	endif ()
endmacro (dump_feature_summary)

macro (prepare_cpack)
	set(CPACK_PROJECT_NAME ${PROJECT_NAME})
	
	# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
	# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
	# set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
	# set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
	include(CPack)
endmacro (prepare_cpack)

macro (add_unit_test target target_dirname target_test)
	# set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${target_test} PARENT_SCOPE)
	# if (${ENABLE_AUTOMATE_TESTS})                #  if (NOT $ENV{CI_RUNNING} AND NOT GCC)
	if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${target_dirname})
		if (${ENABLE_AUTOMATE_TESTS})
			get_property(tmp GLOBAL PROPERTY UNIT_TEST_TARGETS)
			
			# set(tmp ${UNIT_TEST_TARGETS} ${CMAKE_CURRENT_BINARY_DIR}/${target_dirname})
			set(tmp ${UNIT_TEST_TARGETS} ${target_test})
			set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "${tmp}")
		endif ()
		
		message(">> tests enabled for ${target}: adding subdir '${target_dirname}' as target '${target_test}' ")
		add_subdirectory(${target_dirname})
		
		# add_test(target ${CMAKE_CURRENT_BINARY_DIR}/${target_test})
		add_test(NAME ${target_test} COMMAND $<TARGET_FILE:${target_test}>)
		set_property(TARGET ${target_test} PROPERTY FOLDER "Test-projects")
		message(">> add_test(${target_test} -> ${target})")
	endif ()
	
	# endif ()
endmacro (add_unit_test)

macro (apply_all_unit_tests tests_name)
	if (${ENABLE_AUTOMATE_TESTS}) # if (NOT $ENV{CI_RUNNING} AND NOT GCC)
		get_property(tmp GLOBAL PROPERTY UNIT_TEST_TARGETS)
		
		# message("UNIT_TEST_TARGETS: ${tmp}")
		add_custom_target(${tests_name} ALL
				DEPENDS ${tmp}
		)
		add_custom_command(TARGET ${tests_name}
				POST_BUILD COMMAND echo ARGS "   CONFIG = $<CONFIG>, CI_RUNNING = $ENV{CI_RUNNING}, ENABLE_AUTOMATE_TESTS = ${ENABLE_AUTOMATE_TESTS}"
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
		add_custom_command(TARGET ${tests_name}
				COMMENT ">> Run tests for: ${tmp} ..."
				POST_BUILD COMMAND ctest ARGS --output-on-failure --extra-verbose
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	else ()
		message(STATUS "* ENABLE_AUTOMATE_TESTS = ${ENABLE_AUTOMATE_TESTS}, automated tests disabled *")
	endif ()
endmacro ()

function (cxx_set_ipo target)
	#
	# IPO
	#
	# Interprocedural optimization
	# https://cliutils.gitlab.io/modern-cmake/chapters/features/small.html
	if (POLICY CMP0069)
		cmake_policy(SET CMP0069 NEW)
	endif ()
	
	include(CheckIPOSupported) # need decl at first: cmake_minimum_required(VERSION 3.9..3.13)
	check_ipo_supported(RESULT result)
	
	if (result)
		# set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		if (ARGV0)
			set_target_properties("${ARGV0}" PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
			message(STATUS "IPO support enabled for: ${ARGV0}")
		endif ()
	endif ()
endfunction ()

macro (with_flex_bison libName prefixName outputDir)
	if (APPLE)
		list(APPEND CMAKE_PROGRAM_PATH "/usr/local/opt/flex/bin" "/usr/local/opt/bison/bin")
	endif ()
	
	find_package(FLEX 2.6)
	find_package(BISON 2.7)
	
	# FIND_PACKAGE(FLEX 2.5)
	# FIND_PACKAGE(BISON 2.3)
	
	# ----------------------
	# about gcc warnings:
	# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#Warning-Options
	# about cmake doc:
	# http://www.cmake.org/cmake/help/v2.8.8/cmake.html
	add_definitions("-DYYDEBUG=1")
	
	if (outputDir STREQUAL "")
		set(TARGET_DIR "${CMAKE_BINARY_DIR}/generated")
	else ()
		set(TARGET_DIR "${outputDir}")
	endif ()
	
	# IF (NOT ${BISON_EXECUTABLE} STREQUAL "")
	if (BISON_FOUND)
		message(STATUS "BISON ${BISON_VERSION} FOUND: " ${BISON_EXECUTABLE})
		bison_target(${prefixName}Parser
				${CMAKE_CURRENT_SOURCE_DIR}/parser.yy
				${TARGET_DIR}/${prefixName}.parser.cc
				COMPILE_FLAGS "--xml"
				DEFINES_FILE ${TARGET_DIR}/${prefixName}.parser.hh
				VERBOSE ${TARGET_DIR}/${prefixName}.parser.grammer.txt
				REPORT_FILE ${TARGET_DIR}/${prefixName}.parser.grammer.rpt
		)
		
		if (FLEX_FOUND)
			message(STATUS "FLEX ${FLEX_VERSION} FOUND: " ${FLEX_EXECUTABLE} ", INC: " ${FLEX_INCLUDE_DIRS} "|" ${FLEX_INCLUDE_DIR} ", LIBS: " ${FLEX_LIBRARIES})
			
			# message(STATUS "FLEX ${FLEX_VERSION} FOUND: " ${FLEX_EXECUTABLE} ". LIBS: " ${FLEX_LIBRARIES})
			flex_target(${prefixName}Scanner
					${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll
					${TARGET_DIR}/${prefixName}.scanner.cc)
			add_flex_bison_dependency(${prefixName}Scanner ${prefixName}Parser)
			set_source_files_properties(${prefixName}.scanner.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion -Wno-sign-conversion -Wno-sign-compare") # -Wno-int-conversion
		endif ()
		
		set_source_files_properties(${prefixName}.parser.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion -Wno-sign-conversion")
	endif ()
	
	# ###################################
	add_library(${libName} STATIC
			
			# ${THELIB_SRCS}
			${BISON_${prefixName}Parser_OUTPUTS}
			${FLEX_${prefixName}Scanner_OUTPUTS}
	)
	target_link_libraries(${libName}
			
			# ${THELIB_LIBS}
			# pthread
			# ${FLEX_LIBRARIES}
	)
endmacro ()

# Macro to declare a dependent option with more convenient syntax
macro (vg_option NAME DEFAULT_VALUE DOCSTRING DEPENDENCIES)
	cmake_dependent_option(
			${NAME} ${DOCSTRING} ${DEFAULT_VALUE}
			"${DEPENDENCIES}" OFF
	)
endmacro ()

# Function to extract first argument into named variable
function (shift_arg variable value)
	set(${variable} "${value}" PARENT_SCOPE)
	set(ARGN "${ARGN}" PARENT_SCOPE)
endfunction ()

# Helper macro to parse template argument of extract_flags, extract_args
macro (__parse_extract_template template)
	set(_vars "")
	set(_extra "")
	
	foreach (pair ${template})
		string(REGEX MATCH "^[^=]+" varname "${pair}")
		string(REGEX REPLACE "^[^=]+=" "" token "${pair}")
		
		if ("x_${varname}" STREQUAL "x_" OR "x_${token}" STREQUAL "x_")
			message(FATAL_ERROR "extract_args: bad variable/token pair '${pair}'")
		endif ()
		
		set(${varname} "")
		set(__var_for_token_${token} "${varname}")
		list(APPEND _vars "${varname}")
	endforeach ()
endmacro ()

# Function to extract named flags
function (extract_flags template)
	__parse_extract_template("${template}")
	
	foreach (_var ${_vars})
		# Initially set all flags to FALSE
		set(${_var} FALSE PARENT_SCOPE)
	endforeach ()
	
	foreach (arg ${ARGN})
		# Skip empty args
		if (NOT "x_${arg}" STREQUAL "x_")
			# Test if arg is a token
			if (NOT "x_${__var_for_token_${arg}}" STREQUAL "x_")
				# Yes; set flag to TRUE
				set(${__var_for_token_${arg}} TRUE PARENT_SCOPE)
			else ()
				# No; add to leftovers list
				list(APPEND _extra "${arg}")
			endif ()
		endif ()
	endforeach ()
	
	# Shift ARGN
	set(ARGN "${_extra}" PARENT_SCOPE)
endfunction ()

# Function to extract named arguments
function (extract_args template)
	__parse_extract_template("${template}")
	set(_var "")
	
	foreach (arg ${ARGN})
		# Skip empty args
		if (NOT "x_${arg}" STREQUAL "x_")
			# Test if arg is a token
			if (NOT "x_${__var_for_token_${arg}}" STREQUAL "x_")
				set(_var "${__var_for_token_${arg}}")
				
				# Not a token; test if we have a current token
			elseif (NOT "x_${_var}" STREQUAL "x_")
				# Add arg to named list
				list(APPEND ${_var} "${arg}")
			else ()
				# Add arg to leftovers list
				list(APPEND _extra "${arg}")
			endif ()
		endif ()
	endforeach ()
	
	# Raise lists to parent scope
	foreach (varname ${_vars})
		set(${varname} "${${varname}}" PARENT_SCOPE)
	endforeach ()
	
	# Shift ARGN
	set(ARGN "${_extra}" PARENT_SCOPE)
endfunction ()

# Function to extract path to a library given a set of libraries
function (find_libraries_path library_search_paths)
	set(search_paths)
	
	foreach (library ${ARGN})
		get_filename_component(path "${library}" PATH)
		
		if (path)
			set(search_paths ${search_paths} ${path})
		endif (path)
	endforeach ()
	
	set(${library_search_paths} ${search_paths} PARENT_SCOPE)
endfunction ()

# Function to find a library with given name and configuration
function (find_library_for_configuration VAR NAME CONFIG SUFFIX)
	set(paths)
	
	foreach (path ${ARGN})
		list(APPEND paths ${path}/${SUFFIX})
	endforeach ()
	
	set(paths ${paths} ${ARGN})
	find_library(${VAR}_LIBRARY_${CONFIG} ${NAME} PATHS ${paths})
	mark_as_advanced(${VAR}_LIBRARY_${CONFIG})
endfunction ()

# Function to find a library for various configurations
function (find_multi_configuration_library VAR NAME)
	if (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
		find_library_for_configuration(${VAR} ${NAME} RELEASE Release ${ARGN})
		find_library_for_configuration(${VAR} ${NAME} DEBUG Debug ${ARGN})
		
		set(libraries)
		
		if (${VAR}_LIBRARY_RELEASE)
			set(libraries ${libraries} optimized ${${VAR}_LIBRARY_RELEASE})
		endif ()
		
		if (${VAR}_LIBRARY_DEBUG)
			set(libraries ${libraries} debug ${${VAR}_LIBRARY_DEBUG})
		endif ()
		
		if ("x_${libraries}" STREQUAL "x_")
			set(${VAR}_LIBRARY ${VAR}_LIBRARY-NOTFOUND PARENT_SCOPE)
		else ()
			set(${VAR}_LIBRARY ${libraries} PARENT_SCOPE)
		endif ()
	else (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
		find_library(${VAR}_LIBRARY ${NAME}
				PATHS ${ARGN}
		)
	endif (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
endfunction ()

# Function to add definitions to individual source files' compile flags
function (add_source_files_definitions)
	extract_args("_symbols=SYMBOLS;_symbols=DEFINITIONS" ${ARGN})
	
	# Generate list of new compile flags
	set(_extra_flags)
	
	foreach (_symbol ${_symbols})
		set(_extra_flags "${_extra_flags} -D${_symbol}")
	endforeach ()
	
	# Iterate over each file
	foreach (_file ${ARGN})
		# Get current flags
		get_source_file_property(_flags "${_file}" COMPILE_FLAGS)
		
		if ("x_${_flags}" STREQUAL "x_NOTFOUND")
			set(_flags)
		endif ()
		
		# Append new flags
		set(_flags "${_flags} ${_extra_flags}")
		
		# Set new flags
		set_source_files_properties("${_file}" PROPERTIES COMPILE_FLAGS "${_flags}")
	endforeach ()
endfunction ()

# Function to add definitions to individual source files' compile flags
function (add_targets_definitions)
	extract_args("_symbols=SYMBOLS;_symbols=DEFINITIONS" ${ARGN})
	
	# Generate list of new compile flags
	set(_extra_flags)
	
	foreach (_symbol ${_symbols})
		set(_extra_flags "${_extra_flags} -D${_symbol}")
	endforeach ()
	
	# Iterate over each file
	foreach (_target ${ARGN})
		# Get current flags
		get_target_property(_flags "${_target}" COMPILE_FLAGS)
		
		if ("x_${_flags}" STREQUAL "x_NOTFOUND" OR "x_${_flags}" STREQUAL "x__flags-NOTFOUND")
			set(_flags)
		endif ()
		
		# Append new flags
		set(_flags "${_flags} ${_extra_flags}")
		
		# Set new flags
		set_target_properties("${_target}" PROPERTIES COMPILE_FLAGS "${_flags}")
	endforeach ()
endfunction ()

# Function to resolve library names to full paths
function (resolve_library_paths VAR)
	set(_libraries)
	
	foreach (_library ${${VAR}})
		if (IS_ABSOLUTE ${_library})
			list(APPEND _libraries "${_library}")
		else ()
			find_multi_configuration_library(${_library} ${_library} ${ARGN})
			
			if (${_library}_LIBRARY)
				list(APPEND _libraries "${${_library}_LIBRARY}")
				mark_as_advanced(${_library}_LIBRARY)
			else ()
				list(APPEND _libraries "${_library}")
			endif ()
		endif ()
	endforeach ()
	
	set(${VAR} ${_libraries} PARENT_SCOPE)
endfunction ()

# Function to add include directory to target's SDK include directories
function (vg_target_include_directories TARGET)
	set(_changed FALSE)
	
	foreach (_path ${ARGN})
		list(FIND ${TARGET}_INCLUDE_DIRS "${_path}" _index)
		
		if (_index EQUAL -1)
			list(APPEND ${TARGET}_INCLUDE_DIRS "${_path}")
			set(_changed TRUE)
		endif ()
	endforeach ()
	
	if (_changed)
		set(${TARGET}_INCLUDE_DIRS "${${TARGET}_INCLUDE_DIRS}"
				CACHE INTERNAL "SDK include directories for ${TARGET}")
	endif ()
endfunction ()

# Function to include library SDK directories
function (vg_include_library_sdk_directories)
	extract_args("_target=TARGET;_interface_targets=INTERFACE" ${ARGN})
	
	if (NOT "x_${_interface_targets}" STREQUAL "x_")
		if ("x_${_target}" STREQUAL "x_")
			message(FATAL_ERROR "vg_include_library_sdk_directories:"
					" INTERFACE includes specified,"
					" but no TARGET specified")
		endif ()
		
		# Handle interface include directories
		foreach (_interface ${_interface_targets})
			vg_target_include_directories(
					${_target} "${${_interface}_INCLUDE_DIRS}"
			)
			include_directories(${${_interface}_INCLUDE_DIRS})
		endforeach ()
	endif ()
	
	# Handle non-interface include directories
	if (NOT "x_${ARGN}" STREQUAL "x_")
		foreach (_lib ${ARGN})
			include_directories(${${_lib}_INCLUDE_DIRS})
		endforeach ()
	endif ()
endfunction ()

# Function to add include and/or link dependencies to a target
function (vg_add_dependencies TARGET)
	set(_args)
	list(APPEND _args "_link=LINK_LIBRARIES")
	list(APPEND _args "_public_targets=PUBLIC_INTERFACE_TARGETS")
	list(APPEND _args "_private_targets=PRIVATE_INTERFACE_TARGETS")
	extract_args("${_args}" ${ARGN})
	list(APPEND _private_interface_targets ${ARGN})
	
	# Add include directories
	vg_include_library_sdk_directories(
			${_private_targets}
			TARGET ${TARGET} INTERFACE ${_public_targets}
	)
	
	# Link libraries
	if (NOT "x_${_private_targets}" STREQUAL "x_")
		target_link_libraries(${TARGET} LINK_PRIVATE ${_private_targets})
	endif ()
	
	if (NOT "x_${_public_targets}" STREQUAL "x_")
		target_link_libraries(${TARGET} LINK_PUBLIC ${_public_targets})
	endif ()
	
	if (NOT "x_${_link}" STREQUAL "x_")
		target_link_libraries(${TARGET} ${_link})
	endif ()
endfunction ()

# Function to generate a target to copy a list of files to a destination
function (copy_files_target TARGET DESTINATION)
	set(_target_depends)
	
	foreach (file ${ARGN})
		get_filename_component(_in "${file}" REALPATH)
		get_filename_component(_out "${file}" NAME)
		set(_out "${DESTINATION}/${_out}")
		add_custom_command(OUTPUT "${_out}" DEPENDS "${_in}"
				COMMAND ${CMAKE_COMMAND} -E copy "${_in}" "${_out}")
		list(APPEND _target_depends "${_out}")
	endforeach ()
	
	add_custom_target(${TARGET} ALL DEPENDS ${_target_depends})
endfunction ()

# Function to install executable targets
function (install_executable_target NAME COMPONENT)
	if (TARGET ${NAME})
		get_target_property(_type ${NAME} TYPE)
		
		if (_type STREQUAL "EXECUTABLE")
			install(TARGETS ${NAME}
					RUNTIME COMPONENT ${COMPONENT} DESTINATION bin
					BUNDLE COMPONENT ${COMPONENT} DESTINATION .
			)
		else ()
			message(WARNING
					"install_executable_target given non-executable target '${NAME}'"
			)
		endif ()
	else ()
		message(WARNING
				"install_executable_target given non-target name '${NAME}'"
		)
	endif ()
endfunction ()

# Function to install library targets
function (install_library_targets)
	foreach (_target ${ARGN})
		if (TARGET ${_target})
			if (TARGET ${_target}Headers)
				install_library_targets(${_target}Headers)
			endif ()
			
			get_target_property(_type ${_target} TYPE)
			
			if (_type MATCHES "(SHARED|STATIC|MODULE)_LIBRARY")
				install(TARGETS ${_target}
						EXPORT VisGUI
						RUNTIME COMPONENT Runtime DESTINATION bin
						LIBRARY COMPONENT Runtime DESTINATION lib${LIB_SUFFIX}
						ARCHIVE COMPONENT Development DESTINATION lib${LIB_SUFFIX}
				)
				
				if (_type STREQUAL "SHARED_LIBRARY")
					export(TARGETS ${_target} APPEND FILE "${VisGUI_EXPORT_FILE}")
				endif ()
			elseif (_type STREQUAL "INTERFACE_LIBRARY")
				install(TARGETS ${_target} EXPORT VisGUI)
				set_property(GLOBAL APPEND PROPERTY VisGUI_EXPORT_TARGETS ${_target})
			else ()
				message(WARNING
						"install_library_targets given non-library target '${_target}'"
				)
			endif ()
		else ()
			message(WARNING
					"install_library_targets given non-target name '${_target}'"
			)
		endif ()
	endforeach ()
endfunction ()

# Function to define a VTK module
macro (vg_vtk_module NAME)
	# Register module for subsequent use as a dependency
	set_property(GLOBAL APPEND PROPERTY VG_VTK_MODULES ${NAME})
	get_property(VG_VTK_MODULES GLOBAL PROPERTY VG_VTK_MODULES)
	list(APPEND VTK_MODULES_ENABLED ${VG_VTK_MODULES})
	
	# Clear vg include directories; they will be rebuilt, but VTK also picks them
	# up, which causes the VTK module include directories to change on subsequent
	# CMake runs, which can lead to hard-to-track-down bugs where clean builds
	# break, but work after a second configure pass.
	unset(${NAME}_INCLUDE_DIRS CACHE)
	
	vtk_module(${NAME} ${ARGN})
endmacro ()

# Function to define a Qt plugin target
function (vg_add_qt_plugin NAME)
	add_library(${NAME} MODULE ${ARGN})
	get_target_property(_type ${NAME} TYPE)
	add_targets_definitions(${NAME} SYMBOLS QT_PLUGIN QT_SHARED)
	set_target_properties(${NAME} PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
	)
	
	foreach (config ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER "${config}" config_upper)
		set_target_properties(${NAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY_${config_upper}
				${CMAKE_BINARY_DIR}/${config}/plugins
				RUNTIME_OUTPUT_DIRECTORY_${config_upper}
				${CMAKE_BINARY_DIR}/${config}/plugins
		)
	endforeach ()
endfunction ()

# Function to install files with (optionally) their respective path prefixes
function (vg_install_files_with_prefix)
	cmake_parse_arguments(_install
			"STRIP_PATH"
			"SOURCE;TARGET;DESTINATION"
			"FILES"
			${ARGN}
	)
	list(APPEND _install_FILES ${_install_UNPARSED_ARGUMENTS})
	
	set(_path)
	set(_in_prefix)
	
	if (_install_SOURCE)
		set(_in_prefix "${_install_SOURCE}/")
	endif ()
	
	set(_target_depends)
	
	foreach (file ${_install_FILES})
		get_filename_component(_in "${_in_prefix}${file}" REALPATH)
		get_filename_component(_name "${file}" NAME)
		
		if (NOT _install_STRIP_PATH)
			get_filename_component(_path "${file}" PATH)
		endif ()
		
		set(_out "${visGUI_BINARY_DIR}/${_install_DESTINATION}/${_path}/${_name}")
		
		add_custom_command(OUTPUT "${_out}" DEPENDS "${_in}"
				COMMAND ${CMAKE_COMMAND} -E copy "${_in}" "${_out}")
		list(APPEND _target_depends "${_out}")
		
		install(FILES "${_in_prefix}${file}"
				DESTINATION ${_install_DESTINATION}/${_path}
				COMPONENT Web
		)
	endforeach ()
	
	add_custom_target(${_install_TARGET} ALL DEPENDS ${_target_depends})
endfunction ()

# Function to install headers
function (install_headers)
	extract_args("_target=TARGET;_dest=DESTINATION" ${ARGN})
	
	if (NOT _dest)
		set(_dest include)
	endif ()
	
	if ("x_${_target}" STREQUAL "x_")
		message(FATAL_ERROR "install_headers: no TARGET specified")
	endif ()
	
	if (NOT "x_${ARGN}" STREQUAL "x_")
		# Iterate over file list
		foreach (_file ${ARGN})
			# Get name, canonical path and subdirectory
			string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/" "" _relfile "${_file}")
			get_filename_component(_name "${_file}" NAME)
			get_filename_component(_subdir "${_relfile}" PATH)
			get_filename_component(_realpath "${_file}" REALPATH)
			
			if ("${_subdir}" STREQUAL "")
				set(_subdir ".")
			endif ()
			
			# Add to subdirectory group
			string(REGEX REPLACE "[^A-Za-z0-9]" "_" _dirvar "${_subdir}")
			
			if (NOT _dir_${_dirvar})
				list(APPEND _dirs "${_subdir}")
				set(_dir_${_dirvar} "${_file}")
			else ()
				list(APPEND _dir_${_dirvar} "${_file}")
			endif ()
			
			# Create SDK wrapper header
			set(_wrapper "${CMAKE_BINARY_DIR}/${_dest}/${_subdir}/${_name}")
			file(WRITE "${_wrapper}.tmp" "#include \"${_realpath}\"\n")
			execute_process(
					COMMAND ${CMAKE_COMMAND}
					-E copy_if_different "${_wrapper}.tmp" "${_wrapper}"
			)
			file(REMOVE "${_wrapper}.tmp")
		endforeach ()
		
		# Iterate over subdirectory groups and install headers
		foreach (_dir ${_dirs})
			if ("x_${_dir}" STREQUAL "x_.")
				vg_target_include_directories(
						${_target} "${CMAKE_BINARY_DIR}/${_dest}"
				)
				install(FILES ${_dir__}
						COMPONENT Development
						DESTINATION "${_dest}"
				)
			else ()
				string(REGEX REPLACE "[^A-Za-z0-9]" "_" _dirvar "${_dir}")
				vg_target_include_directories(
						${_target} "${CMAKE_BINARY_DIR}/${_dest}/${_dir}"
				)
				install(FILES ${_dir_${_dirvar}}
						COMPONENT Development
						DESTINATION "${_dest}/${_dir}"
				)
			endif ()
		endforeach ()
	endif ()
endfunction ()

# Function to add a library with an include-only interface
function (vg_add_library NAME)
	add_library(${NAME}Headers INTERFACE)
	add_library(${NAME} ${ARGN})
	target_link_libraries(${NAME} PUBLIC ${NAME}Headers)
endfunction ()

# Function to install plugin targets
function (install_plugin_targets)
	foreach (_target ${ARGN})
		if (TARGET ${_target})
			get_target_property(_type ${_target} TYPE)
			
			if (NOT _type STREQUAL "STATIC_LIBRARY")
				install(TARGETS ${_target}
						COMPONENT Plugins
						RUNTIME DESTINATION plugins
						LIBRARY DESTINATION plugins)
			endif ()
		endif ()
	endforeach ()
endfunction ()

# https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory

# Usage:
# subdir_list(${MY_CURRENT_DIR} SUBDIRS)

# Or foreach:
#
# FOREACH(subdir ${SUBDIRS})
# ADD_SUBDIRECTORY(${subdir})
# ENDFOREACH()
function (subdir_list result curdir)
	# message("curdir = ${curdir}")
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	
	foreach (child ${children})
		if (IS_DIRECTORY ${curdir}/${child})
			list(APPEND dirlist ${child})
		endif ()
	endforeach ()
	
	set(${result} ${dirlist} PARENT_SCOPE)
endfunction ()

# cmake_subdir_list lists all subdirectories which contains CMakeLists.txt
function (cmake_subdir_list result curdir)
	# message("curdir = ${curdir}")
	file(GLOB children
			LIST_DIRECTORIES true
			RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	
	foreach (child ${children})
		if (EXISTS ${curdir}/${child}/CMakeLists.txt)
			list(APPEND dirlist ${child})
		endif ()
	endforeach ()
	
	set(${result} ${dirlist} PARENT_SCOPE)
endfunction ()
