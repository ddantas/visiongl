# - Try to find FreeGLUT
# Once done this will define
#  FREEGLUT_FOUND - System has FreeGLUT
#  FREEGLUT_INCLUDE_DIR - The FreeGLUT include directory
#  FREEGLUT_LIBRARY - The library needed to use FreeGLUT

message("Looking for Freeglut")
find_package(FREEGLUT QUIET)

if (NOT ${FREEGLUT_FOUND})
	message("Package Freeglut not found, trying to find it somewhere.")
        if (WIN32)
		# If find_package is not finding Glew, please add here the path to Glew folder
		set(possible_paths 
			# win32 custom path
			"$ENV{userprofile}/Documents"
			"c:/lib/freeglut_MSVC"
		)

		find_path(FREEGLUT_DIR NAMES "include/GL/freeglut.h"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "INCLUDE" "freeglut/include"
			)
		
		find_path(FREEGLUT_INCLUDE_DIR NAMES "GL/freeglut.h"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "INCLUDE" "freeglut/include"
			)
		
		find_library(FREEGLUT_LIBRARY NAMES "freeglut.lib"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "freeglut" "LIB" "freeglut/lib"
			)
			
		if (NOT (${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND" AND ${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND"))
			set(FREEGLUT_FOUND TRUE)
			message("Glew was found after some tries")
		endif()

		if(NOT ${FREEGLUT_FOUND})
			message("Freeglut lib and include not found. Please add Freeglut custom path to cmake-modules/FindMyFREEGLUT.cmake, or set these variables manually as a cmake parameter like -DFREGLUT_INCLUDE_DIR=c:/freeglut/include")

			if(${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND")
				set(FREEGLUT_INCLUDE_DIR CACHE PATH "Path to the FREEGLUT include directory")
			endif()
		
			if(${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND")
				set(FREEGLUT_LIBRARY CACHE FILEPATH "Path to the FREEGLUT library freeglut.lib")
			endif()
			
		endif()
        else()
		# If find_package is not finding Glew, please add here the path to Freeglut folder
		set(possible_paths 
		  "/usr/"
		  "/usr/include"
		  "/usr/lib"
		  "/usr/lib/x86_64-linux-gnu"
		  # linux custom path
		)
		
		find_path(FREEGLUT_INCLUDE_DIR NAMES GL/freeglut.h
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLUT" "INCLUDE" "GLUT/include"
			)
		
		find_library(FREEGLUT_LIBRARY 
		    NAMES libglut.so
			PATHS ${possible_paths} 
			PATH_SUFFIXES "GLUT" "LIB" "GLUT/lib"
			)
		
		if (NOT (${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND" AND ${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND"))
			set(FREEGLUT_FOUND TRUE)
			message("Glew was found after some tries")
		endif()

		if(NOT ${FREEGLUT_FOUND})
			
			if(${FREEGLUT_INCLUDE_DIR} STREQUAL "FREEGLUT_INCLUDE_DIR-NOTFOUND")
				set(FREEGLUT_INCLUDE_DIR CACHE PATH "Path to the FREEGLUT include directory")
			endif()
		
			if(${FREEGLUT_LIBRARY} STREQUAL "FREEGLUT_LIBRARY-NOTFOUND")
				set(FREEGLUT_LIBRARY CACHE FILEPATH "Path to the FREEGLUT library libglut.so")
			endif()
			
		endif()
        endif()
else()
	message("FreeGLUT lib and include found.")
endif()

message("Freeglut include: " ${FREEGLUT_INCLUDE_DIR})
message("Freeglut library: " ${FREEGLUT_LIBRARY})
message("")
