# - Try to find GLEW
# Once done this will define
#  GLEW_FOUND - System has GLEW
#  GLEW_INCLUDE_DIR - The GLEW include directory
#  GLEW_LIBRARY - The library needed to use GLEW

message("Looking for Glew")
find_package(GLEW)

if (NOT ${GLEW_FOUND})
	message("Package Glew not found, trying to find it somewhere.")
	if (WIN32)
		# If find_package is not finding Glew, please add here the path to Glew folder
		set(possible_paths
			# win32 custom path
			"$ENV{userprofile}/Documents"
			"C:/lib/glew-1.11.0"
		)

		find_path(GLEW_INCLUDE_DIR NAMES "GL/glew.h"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "INCLUDE" "GLEW/include")
		
		find_library(GLEW_LIBRARY NAMES "glew32.lib"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "win32" "GLEW/lib/release/Win32" "lib/release/Win32")
		
		if (NOT (${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND" AND ${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND"))
			set(GLEW_FOUND TRUE)
			message("Glew was found after some tries")
		endif()

		if(NOT GLEW_FOUND)
			message("Glew lib and include not found. Please add glew custom path to cmake-modules/FindMyGLEW.cmake, or set these variables manually as a cmake parameter like -DGLEW_INCLUDE_DIR=c:/glew/include")

			if(${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND")
				set(GLEW_INCLUDE_DIR CACHE PATH "Path to the GLEW include directory")
			endif()
		
			if(${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND")
				set(GLEW_LIBRARY CACHE FILEPATH "Path to the GLEW library glew32.lib")
			endif()
			
		endif()
		
	else()
		# If find_package is not finding Glew, please add here the path to Glew folder
		set(possible_paths 
		  "/usr/"
		  "/usr/lib"
		  "/usr/include"
		  # linux custom path
		  "/home/visiongl/visiongl/glew"
		)
		
		find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "INCLUDE" "glew/include"
			)
		
		find_library(GLEW_LIBRARY NAMES libGLEW.so
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLEW" "LIB" "release" "glew/lib" "x86_64-linux-gnu"
			)
		
		if (NOT (${GLEW_INCLUDE_DIR} STREQUAL "GLEW_INCLUDE_DIR-NOTFOUND" AND ${GLEW_LIBRARY} STREQUAL "GLEW_LIBRARY-NOTFOUND"))
			set(GLEW_FOUND TRUE)
			message("Glew was found after some tries")
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
	message("Glew lib and include found.")
endif()

message("Glew include: " ${GLEW_INCLUDE_DIR})
message("Glew library: " ${GLEW_LIBRARY})
message("")
