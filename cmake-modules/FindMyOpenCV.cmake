# - Try to find OpenCV
# Once done this will define
#  OPENCV_FOUND - System has OpenCV
#  OPENCV_INCLUDE_DIR - The OpenCV include directory
#  OPENCV_LIBRARY - The library dir needed to use OpenCV
cmake_minimum_required(VERSION 2.5)

find_package(OpenCV QUIET)
if(NOT OpenCV_FOUND)
	message("trying to find OpenCV")
	if(WIN32)
		set(possible_paths
			$ENV{path}
			$ENV{programfiles}
			#mycustompath
			$ENV{userprofile}/Documents
		)
		
		find_path(OPENCV_INCLUDE_DIR opencv/cv.h
				PATHS ${possible_paths}
				PATH_SUFFIXES "opencv" "include" "opencv/include"
		)
		
		find_path(OPENCV_LIBRARY NAMES opencv_highgui246.lib 
			PATHS ${possible_paths} 
			PATH_SUFFIXES "opencv" "lib" "opencv/lib" "opencv/build/x86/vc10/lib"
		)
		
		message("OpenCV Include: " ${OPENCV_INCLUDE_DIR})
		message("OpenCV Library: " ${OPENCV_LIBRARY})
		
		if (NOT (${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND" AND ${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND"))
			set(OPENCV_FOUND TRUE)
			message("opencv was found after some tries")
		endif()

		if(NOT OPENCV_FOUND)
			
			if(${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND")
				set(OPENCV_INCLUDE_DIR CACHE PATH "Path to the OpenCV include directory")
			endif()
		
			if(${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND")
				set(OPENCV_LIBRARY CACHE PATH "Path to the OpenCV library folder")
			endif()
			
		endif()
		
	else()
	
		set(possible_paths 
		  "/usr/"
		  "/usr/lib"
		  "/usr/include"
		  "/usr/lib/x86_64-linux-gnu"
		  "/usr/local"
		  #mycustompath
		)
		
		find_path(OPENCV_INCLUDE_DIR opencv/cv.h
				PATHS ${possible_paths}
				PATH_SUFFIXES "opencv" "include" "opencv/include"
		)
		
		find_path(OPENCV_LIBRARY NAMES libopencv_highgui.so
			PATHS ${possible_paths} 
			PATH_SUFFIXES "opencv" "lib" "opencv/lib" "opencv/build/x86/vc10/lib"
		)
		
		message("OpenCV Include: " ${OPENCV_INCLUDE_DIR})
		message("OpenCV Library: " ${OPENCV_LIBRARY})
		
		if (NOT (${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND" AND ${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND"))
			set(OPENCV_FOUND TRUE)
			message("opencv was found after some tries")
		endif()

		if(NOT OPENCV_FOUND)
			
			if(${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND")
				set(OPENCV_INCLUDE_DIR CACHE PATH "Path to the OpenCV include directory")
			endif()
		
			if(${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND")
				set(OPENCV_LIBRARY CACHE PATH "Path to the OpenCV library folder")
			endif()
			
		endif()

	endif()
	
else()
	

endif()
