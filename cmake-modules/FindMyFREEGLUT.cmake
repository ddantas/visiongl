# - Try to find GLEW
# Once done this will define
#  FREEGLUT_FOUND - System has GLEW
#  FREEGLUT_INCLUDE_DIR - The GLEW include directory
#  FREEGLUT_LIBRARY - The library needed to use GLEW
cmake_minimum_required(VERSION 2.5)
message("started finding freeglut")
if (WIN32)
	find_package(FREEGLUT QUIET)
	if (NOT FREEGLUT_FOUND)
		message("Package freeglut not found, trying to find it somewhere, if you have any trouble, edit FindMyFREEGLUT.cmake in cmake-modules directory")
	
		#please add here a possible path for you computer to find GLEW 
		#if find_package isnt working // remembering, this paths are windows only
		set(possible_paths 
		  $ENV{programfiles}
		  $ENV{path}
		  #mycustompath
		  "$ENV{userprofile}/Documents"
		)
		
		find_path(FREEGLUT_DIR NAMES include/gl/freeglut.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "INCLUDE" "freeglut/include"
			)
		
		find_path(FREEGLUT_INCLUDE_DIR NAMES gl/freeglut.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "INCLUDE" "freeglut/include"
			)
		
		find_library(FREEGLUT_LIBRARY NAMES freeglut.lib 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "LIB" "freeglut/lib"
			)
		
		if (NOT (${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND" AND ${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND"))
			set(FREEGLUT_FOUND TRUE)
			message("Managed to find FREEGLUT")
		endif()

		if(NOT FREEGLUT_FOUND)
			
			if(${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND")
				set(FREEGLUT_INCLUDE_DIR CACHE PATH "Path to the FREEGLUT include directory")
			endif()
		
			if(${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND")
				set(FREEGLUT_LIBRARY CACHE FILEPATH "Path to the FREEGLUT library freeglut.lib")
			endif()
			
		endif()
	else()
		message("Glew lib and include found successful")
	endif()
else()
	find_package(GLUT)
	if (NOT GLUT_FOUND)
		message("Package freeglut not found, trying to find it somewhere, if you have any trouble, edit FindGlew.cmake in cmake-modules directory")
		#please add here a possible path for you computer to find GLEW 
		#if find_package isnt working // remembering, this paths are windows only
		set(possible_paths 
		  "/usr/"
		  "/usr/include"
		  "/usr/lib"
		  "/usr/lib/x86_64-linux-gnu"
		  #mycustompath
		)
		
		
		find_path(FREEGLUT_INCLUDE_DIR NAMES gl/freeglut.h
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLUT" "INCLUDE" "GLUT/include"
			)
		
		find_library(FREEGLUT_LIBRARY 
		    NAMES libglut.so libGLEW.so.3 libGLEW.so.3.9.0
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "glew/lib"
			)
		
		message("glew include: " ${glut_include_dir})
		message("glew library: " ${glut_library})
		
		if (NOT (${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND" AND ${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND"))
			set(FREEGLUT_FOUND TRUE)
			message("Managed to find FREEGLUT")
		endif()

		if(NOT FREEGLUT_FOUND)
			
			if(${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND")
				set(FREEGLUT_INCLUDE_DIR CACHE PATH "Path to the FREEGLUT include directory")
			endif()
		
			if(${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND")
				set(FREEGLUT_LIBRARY CACHE FILEPATH "Path to the FREEGLUT library libglut.so")
			endif()
			
		endif()
	else()
		set(FREEGLUT_FOUND TRUE)
		set(FREEGLUT_INCLUDE_DIR ${glut_include_dir})
		set(FREEGLUT_LIBRARY ${glut_libraries})
	endif()
endif()