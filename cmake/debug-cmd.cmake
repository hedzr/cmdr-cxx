
macro (debug msg)
	message(STATUS "[DEBUG] ${msg}")
endmacro ()

macro (debug_print_value variableName)
	debug("${variableName} = \${${variableName}}")
endmacro ()

macro (debug_print_list_value listName)
	message(STATUS "- List of ${listName} -------------")
	
	foreach (lib ${${listName}})
		message(STATUS "                         ${lib}")
	endforeach (lib)
	
	message(STATUS "- end -")
endmacro ()

macro (safe_include cmakefilepath short_desc)
	if (EXISTS ${CMAKE_SOURCE_DIR}/${cmakefilepath})
		include(${CMAKE_SOURCE_DIR}/${cmakefilepath})
		message(STATUS ">>> ${short_desc} found and loaded: ./${cmakefilepath}")
	else ()
		message(STATUS ">>> ${short_desc} ignored")
	endif ()
endmacro ()
