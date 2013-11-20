# - Try to find GLEW
# Once done this will define
#  GLEW_FOUND - System has GLEW
#  GLEW_INCLUDE_DIR - The GLEW include directory
#  GLEW_LIBRARY - The library needed to use GLEW
cmake_minimum_required(VERSION 2.7)
find_package(GLEW)
if (NOT GLEW_FOUND)
	if (WIN32)
		#please add here a possible path for you computer to find GLEW 
		#if find_package isnt working // remembering, this paths are windows only
		set(possible_paths 
		  "C:/Program Files/Microsoft Visual Studio"
		  "$ENV{programfiles}/Microsoft Visual Studio"
		  $ENV{programfiles}
		  $ENV{path}
		  #mycustompath
		  "$ENV{userprofile}/Documents"
		)
		
		find_path(GLEW_INCLUDE_DIR NAMES gl/glew.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "INCLUDE" "glew/include"
			)
		
		find_library(GLEW_LIBRARY NAMES glew32.lib 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "win32" "glew/lib/release/win32"
			)
		
		if (NOT (${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND" AND ${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND"))
			set(GLEW_FOUND TRUE)
		endif()

		if(NOT GLEW_FOUND)
			
			if(${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND")
				set(GLEW_INCLUDE_DIR CACHE PATH "Path to the GLEW include directory")
			endif()
		
			if(${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND")
				set(GLEW_LIBRARY CACHE FILEPATH "Path to the GLEW library glew32.lib")
			endif()
			
		endif()

	else()
		#please add here a possible path for you computer to find GLEW 
		#if find_package isnt working // remembering, this paths are windows only
		set(possible_paths 
		  "/usr/"
		  "/usr/lib"
		  "/usr/lib/x86_64-linux-gnu"
		  #mycustompath
		  "/home/visiongl/visiongl/glew"
		  
		)
		
		find_path(GLEW_INCLUDE_DIR NAMES gl/glew.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "INCLUDE" "glew/include"
			)
		
		find_library(GLEW_LIBRARY 
		    NAMES libGLEW.so.1.8 libGLEW.so.1.7 libGLEW.so.1.6
			      libGLEW.so.1.8.0 libGLEW.so.1.7.0 libGLEW.so.1.6.0
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "glew/lib"
			)
		
		message("glew include: " ${glew_include_dir})
		message("glew library: " ${glew_library})
		
		if (NOT (${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND" AND ${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND"))
			set(GLEW_FOUND TRUE)
		endif()

		if(NOT GLEW_FOUND)
			
			if(${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND")
				set(GLEW_INCLUDE_DIR CACHE PATH "Path to the GLEW include directory")
			endif()
		
			if(${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND")
				set(GLEW_LIBRARY CACHE FILEPATH "Path to the GLEW library glew32.lib")
			endif()
			
		endif()
	endif()
endif()
