# - Try to find GLEW
# Once done this will define
#  GLEW_FOUND - System has GLEW
#  GLEW_INCLUDE_DIR - The GLEW include directory
#  GLEW_LIBRARY - The library needed to use GLEW
find_package(GLEW)
if (NOT ${GLEW_FOUND})
	message("Package Glew not found, trying to find it somewhere, if you have any trouble, edit FindMyGLEW.cmake in cmake-modules directory")
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
			message("Glew was found after some tries")
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
		  "/usr/include"
		  #mycustompath
		  "/home/visiongl/visiongl/glew"
		  
		)
		
		find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "INCLUDE" "glew/include"
			)
		
		find_library(GLEW_LIBRARY 
            NAMES libGLEW.so
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "glew/lib" "x86_64-linux-gnu"
			)
		
		message("glew include: " ${GLEW_INCLUDE_DIR})
		message("glew library: " ${GLEW_LIBRARY})
		
		if (NOT (${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND" AND ${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND"))
			set(GLEW_FOUND TRUE)
		endif()

		if(NOT ${GLEW_FOUND})
			
			if(${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND")
				set(GLEW_INCLUDE_DIR CACHE PATH "Path to the GLEW include directory")
			endif()
		
			if(${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND")
				set(GLEW_LIBRARY CACHE FILEPATH "Path to the GLEW library libGLEW.so")
			endif()
			
		endif()
	endif()
else()
	message("Glew lib and include found successful")
endif()