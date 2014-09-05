# - Try to find OpenCV
# Once done this will define
#  OPENCV_FOUND - System has OpenCV
#  OPENCV_INCLUDE_DIR - The OpenCV include directory
#  OPENCV_LIBRARY - The OpenCV lib directory
#  OPENCV_LIBRARIES - List of OpenCV libraries with absolute path

message("Looking for Opencv version ${OpenCV_VERSION}")
find_package(OpenCV)

if(NOT ${OpenCV_FOUND})
	message("Package Opencv not found, trying to find it somewhere.")
	if(WIN32)
		# If find_package is not finding Opencv, please add here the path to Opencv folder
		set(possible_paths
			$ENV{path}
			$ENV{programfiles}
			# win32 custom path
			$ENV{userprofile}/Documents
			"c:/lib/opencv/build"
		)
		
		find_path(OPENCV_INCLUDE_DIR NAMES "opencv/cv.h"
				PATHS ${possible_paths}
				PATH_SUFFIXES "opencv" "include"
		)
		
		find_path(OPENCV_LIBRARY NAMES "opencv_highgui${OpenCV_VERSION}.lib"
			PATHS ${possible_paths} 
			PATH_SUFFIXES "opencv" "lib" "opencv/lib" "x86/vc10/lib" "x86/vc11/lib" "x86/vc12/lib"
		)
		
		if (NOT (${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND" OR ${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND"))
			set(OPENCV_FOUND TRUE)
			message("Opencv was found after some tries")
		endif()

		if(NOT ${OPENCV_FOUND})
			
			if(${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND")
				set(OPENCV_INCLUDE_DIR CACHE PATH "Path to the OpenCV include directory")
			endif()
		
			if(${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND")
				set(OPENCV_LIBRARY CACHE PATH "Path to the OpenCV library folder")
			endif()
			
		endif()
		
	else()
		# If find_package is not finding Opencv, please add here the path to Opencv folder
		set(possible_paths 
		  "/usr/"
		  "/usr/lib"
		  "/usr/include"
		  "/usr/lib/x86_64-linux-gnu"
		  "/usr/local"
		  # linux custom path
		)
		
		find_path(OPENCV_INCLUDE_DIR opencv/cv.h
				PATHS ${possible_paths}
				PATH_SUFFIXES "opencv" "include" "opencv/include"
		)
		
		find_path(OPENCV_LIBRARY NAMES libopencv_highgui.so
			PATHS ${possible_paths} 
			PATH_SUFFIXES "opencv" "lib" "opencv/lib"
		)
		
		if (NOT (${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND" AND ${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND"))
			set(OPENCV_FOUND TRUE)
			message("Opencv was found after some tries")
		endif()

		if(NOT ${OPENCV_FOUND})
			
			if(${OPENCV_INCLUDE_DIR} STREQUAL "OPENCV_INCLUDE_DIR-NOTFOUND")
				set(OPENCV_INCLUDE_DIR CACHE PATH "Path to the OpenCV include directory")
			endif()
		
			if(${OPENCV_LIBRARY} STREQUAL "OPENCV_LIBRARY-NOTFOUND")
				set(OPENCV_LIBRARY CACHE PATH "Path to the OpenCV library folder")
			endif()
			
			message("opencv not found, set OPENCV_INCLUDE_DIR and OPENCV_LIBRARY cache variables")
			
		endif()
	endif()	
else()
	set(OPENCV_LIBRARY ${OpenCV_LIB_DIR_DBG})
	set(OPENCV_INCLUDE_DIR "${OpenCV_CONFIG_PATH}/include")

	message("Opencv lib and include found.")
endif()

if(${OPENCV_FOUND})

        	
	if(WIN32)
		set(OPENCV_LIBRARIES "${OPENCV_LIBRARY}/opencv_core${OpenCV_VERSION}.lib"
				     "${OPENCV_LIBRARY}/opencv_highgui${OpenCV_VERSION}.lib"
				     "${OPENCV_LIBRARY}/opencv_imgproc${OpenCV_VERSION}.lib"
				     "${OPENCV_LIBRARY}/opencv_legacy${OpenCV_VERSION}.lib"
		)
        else()
		set(OPENCV_LIBRARIES ${OPENCV_LIBRARY}/libopencv_core.so
			             ${OPENCV_LIBRARY}/libopencv_highgui.so
				     ${OPENCV_LIBRARY}/libopencv_imgproc.so
				     ${OPENCV_LIBRARY}/libopencv_legacy.so
		)
        endif()

endif()

message("OpenCV include: " ${OPENCV_INCLUDE_DIR})
message("OpenCV library: " ${OPENCV_LIBRARY})
message("OpenCV libraries: " ${OPENCV_LIBRARIES})
message("")

