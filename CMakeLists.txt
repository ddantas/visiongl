# CMakeLists.txt to generate Makefile
# cmake version 2.8 required
cmake_minimum_required(VERSION 2.8)

# Defining project name
project(visiongl)



if(NOT DEFINED OpenCV_VERSION)
	message("No OpenCV_VERSION was given, if you want to set your own OpenCV Version in order to work, please type: cmake . -DOpenCV_VERSION=xxx")
	message("default OpenCV_VERSION: 246")
	set(OpenCV_VERSION 246 CACHE FILEPATH "Version of OpenCV without dots")
endif()

#creating target subdirectories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib)

#creating links to FS
execute_process(COMMAND ln -s ${PROJECT_SOURCE_DIR}/src/FS FS ERROR_QUIET)
execute_process(COMMAND ln -s ${PROJECT_SOURCE_DIR}/src/FS bin/FS ERROR_QUIET)

# visiongl_src stores the name of the source files
set(visiongl_src src/vglImage.cpp src/vglSimpleBGModel.cpp src/vglLoadShader.cpp src/vglContext.cpp src/glsl2cpp_Stereo.cpp src/glsl2cpp_shaders.cpp src/glsl2cpp_DiffEq.cpp src/glsl2cpp_BG.cpp)
set(visiongl_headers src/vglTail.h src/vglSimpleBGModel.h src/vglLoadShader.h src/vglImage.h src/vglHead.h src/vglContext.h src/glsl2cpp_Stereo.h src/glsl2cpp_shaders.h src/glsl2cpp_DiffEq.h src/glsl2cpp_BG.h)
set(visiongl_src_cuda src/vglCudaImage.cu src/kernel2cu_shaders.cu src/kernel2cu_shaders.h src/vglCudaImage.h)
set(visiongl_src_cl src/vglClImage.cpp src/cl2cpp_shaders.cpp)
set(visiongl_headers_cl src/vglClImage.h src/cl2cpp_shaders.h)
set(visiongl_src_GDCM src/vglGdcmIo.cpp src/vglGdcmIo.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")

#Set all input variables as cache for better usability
set(WITH_CUDA CACHE BOOL "Check this if you want to build with CUDA")
set(WITH_OPENCL CACHE BOOL "Check this if you want to build with OpenCL")
set(WITH_GDCM CACHE BOOL "Check this if you want to build with GDCM and 3d Medical Image support")

set(all_libs "")


if(WITH_GDCM)
	find_package (GDCM REQUIRED)
	include(${GDCM_USE_FILE})

        SET(GDCM_LIBRARIES 
            gdcmcharls
            gdcmCommon
            gdcmDICT
            gdcmDSED
            gdcmexpat
            gdcmgetopt
            gdcmIOD
            gdcmjpeg12
            gdcmjpeg16
            gdcmjpeg8
            gdcmMEXD
            gdcmMSFF
            gdcmopenjpeg
            gdcmzlib)
	list(APPEND all_libs ${GDCM_LIBRARIES})
	list(APPEND visiongl_src ${visiongl_src_GDCM})
	add_definitions(-D__GDCM__)
	add_definitions(-fPIC)
	message(${GDCM_USE_FILE})
endif()

# Generate Makefile for Windows
if(WIN32)

	# Looking for lib and include folders of glew, glut and opencv
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (MyOpenCV REQUIRED)
	#set(visiongl_OpenCV_libs opencv_core248d.lib
	#opencv_highgui248d.lib
	#opencv_imgproc248d.lib
	#opencv_legacy248d.lib
	#opencv_features2d248d.lib
	#opencv_calib3d248d.lib)

	# Adding folders containing libs
	link_directories(${OPENCV_LIBRARY})
	

	# Adding folders containing header files
	INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR})
	list(APPEND all_libs ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENCV_LIBRARIES})
	# Add CUDA support if required by user
	if (${WITH_CUDA} AND ${WITH_OPENCL})
		find_package(CUDA REQUIRED)
		find_package(MyOpenCL REQUIRED)

		add_definitions(-D__OPENCL__)
		add_definitions(-D__CUDA__)

		list(APPEND CUDA_NVCC_FLAGS ${custom_cuda_flags})
		list(APPEND CUDA_NVCC_FLAGS -Xcompiler)
		list(APPEND all_libs ${OPENCL_LIBRARY})

		#link_directories(${OPENCL_LIBRARY})
		INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR})

		#_add_library(visiongl ${visiongl_src_cuda})
		cuda_add_library(visiongl ${visiongl_src} ${visiongl_headers} ${visiongl_src_cl} ${visiongl_headers_cl} ${visiongl_src_cuda})

		target_link_libraries(visiongl ${all_libs} )	

	elseif (${WITH_CUDA})
		find_package(CUDA REQUIRED)
		add_definitions(-D__CUDA__)

		list(APPEND CUDA_NVCC_FLAGS ${custom_cuda_flags})
		list(APPEND CUDA_NVCC_FLAGS -Xcompiler)

		cuda_add_library(visiongl ${visiongl_src} ${visiongl_headers} ${visiongl_src_cuda})
	# Creating object file of the library without CUDA
	elseif(${WITH_OPENCL}) # Add OpenCL support if required by user
		find_package (MyOpenCL REQUIRED)
		add_definitions(-D__OPENCL__)
		list(APPEND all_libs ${OPENCL_LIBRARY})

		#link_directories(${OPENCL_LIBRARY})
		INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR})

		add_library(visiongl ${visiongl_src} ${visiongl_headers} ${visiongl_src_cl} ${visiongl_headers_cl})
		target_link_libraries(visiongl ${all_libs})

		# Creating demobenchmarkcl object files then linking it.
		add_executable(demobenchmarkcl src/demo/benchmark_cl.cpp src/demo/timer.h src/demo/timer.cpp)
		SET_TARGET_PROPERTIES(demobenchmarkcl PROPERTIES PREFIX "bin/")
		target_link_libraries(demobenchmarkcl visiongl ${all_libs})
	else() # Creating object file without CUDA and OpenCL
		add_library(visiongl ${visiongl_src} ${visiongl_headers})
	endif()
	# Creating demo_fractal object files
	add_executable(demo_fractal src/demo/fractal.cpp)
			SET_TARGET_PROPERTIES(demo_fractal PROPERTIES PREFIX "bin/")

	# Creating demo_cam object files
	add_executable(demo_cam src/demo/cam.cpp)
			SET_TARGET_PROPERTIES(demo_cam PROPERTIES PREFIX "bin/")
	 
	# Linking executables
	target_link_libraries(demo_cam visiongl ${all_libs})
	target_link_libraries(demo_fractal visiongl ${all_libs})


# Gererate Makefile for Linux
else()

	# find_package(PkgConfig)
	# pkg_check_modules(OpenCL opencl)
	find_package(MyOpenCL REQUIRED)
	# Looking for lib and include folders of glew, glut, opencv and opengl
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (MyOpenCV REQUIRED)
	find_package (OpenGL REQUIRED)

	if(${WITH_GDCM})
		set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} -fPIC)
	endif()

	set(visiongl_OpenCV_libs libopencv_core.so
	libopencv_highgui.so
	libopencv_imgproc.so
	libopencv_legacy.so
	libopencv_features2d.so
	libopencv_calib3d.so)

	list(APPEND all_libs ${visiongl_OpenCV_libs})
	list(APPEND all_libs ${OPENCV_LIBRARIES} ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})
	
	# Adding folders containing libs
	link_directories(${OPENCV_LIBRARY})

	# Adding folders containing header files
	INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR})

	# Add CUDA and OpenCL support if required by user
	if (${WITH_CUDA} AND ${WITH_OPENCL})
		find_package(CUDA REQUIRED)
		find_package (MyOpenCL REQUIRED)

		add_definitions(-D__OPENCL__)
		add_definitions(-D__CUDA__)

		list(APPEND CUDA_NVCC_FLAGS ${custom_cuda_flags})
		list(APPEND CUDA_NVCC_FLAGS -Xcompiler)
		list(APPEND all_libs ${OPENCL_LIBRARY})

		#link_directories(${OPENCL_LIBRARY})
		INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR} ${X11_X11_INCLUDE_PATH})

		cuda_add_library(visiongl SHARED ${visiongl_src} ${visiongl_headers} ${visiongl_src_cl} ${visiongl_headers_cl} ${visiongl_src_cuda} )

		target_link_libraries(visiongl ${all_libs} ${X11_X11_LIB})	
	elseif (${WITH_CUDA}) #Add only CUDA support if required by User
		find_package(CUDA REQUIRED)
		add_definitions(-D__CUDA__)

		list(APPEND CUDA_NVCC_FLAGS ${custom_cuda_flags})
		list(APPEND CUDA_NVCC_FLAGS -Xcompiler)

		cuda_add_library(visiongl SHARED ${visiongl_src} ${visiongl_headers} ${visiongl_src_cuda})
	elseif (${WITH_OPENCL}) #Add only OpenCL support if required by User
		find_package (MyOpenCL REQUIRED)
		add_definitions(-D__OPENCL__)
		list(APPEND all_libs ${OPENCL_LIBRARY})

		#link_directories(${OPENCL_LIBRARY})
		INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR} ${OPENCL_INCLUDE_DIR})

		add_library(visiongl SHARED ${visiongl_src} ${visiongl_headers} ${visiongl_src_cl} ${visiongl_headers_cl} )
		target_link_libraries(visiongl ${all_libs} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${X11_X11_LIB})

		# Creating demobenchmarkcl object files then linking it.
		add_executable(demobenchmarkcl src/demo/benchmark_cl.cpp src/demo/timer.h src/demo/timer.cpp)
		SET_TARGET_PROPERTIES(demobenchmarkcl PROPERTIES PREFIX "bin/")
		target_link_libraries(demobenchmarkcl visiongl ${all_libs})
	else() # Creating object file of the library without CUDA or OpenCL
		add_library(visiongl SHARED ${visiongl_src} ${visiongl_headers})
		target_link_libraries(visiongl ${all_libs})
	endif()
	
	SET_TARGET_PROPERTIES(visiongl PROPERTIES PREFIX "lib/lib")

	# Creating demo_fractal object files
	add_executable(demo_fractal src/demo/fractal.cpp)
			SET_TARGET_PROPERTIES(demo_fractal PROPERTIES PREFIX "bin/")

	# Creating demo_cam object files
	add_executable(demo_cam src/demo/cam.cpp)
			SET_TARGET_PROPERTIES(demo_cam PROPERTIES PREFIX "bin/")

	# Linking executables
	target_link_libraries(demo_fractal visiongl ${all_libs})
	target_link_libraries(demo_cam visiongl ${all_libs})

endif()