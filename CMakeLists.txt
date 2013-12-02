# CMakeLists.txt to generate Makefile
# cmake version 2.8 required
cmake_minimum_required(VERSION 2.8)

# Defining project name 
project(visiongl)

# visiongl_src stores the name of the source files
set(visiongl_src src/vglImage.cpp src/vglSimpleBGModel.cpp src/vglLoadShader.cpp src/vglContext.cpp src/glsl2cpp_Stereo.cpp src/glsl2cpp_shaders.cpp src/glsl2cpp_DiffEq.cpp src/glsl2cpp_BG.cpp)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")

# Gererate Makefile for Windows
if(WIN32)

	# Looking for lib and include folders of glew, glut and opencv
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (MyOpenCV REQUIRED)

	# Adding folders containing libs
	link_directories(${OPENCV_LIBRARY})

	# Adding folders containing header files
	INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR})

	# Creating object file of the library
	add_library(visiongl ${visiongl_src})

	# Creating demo_fractal and demo_cam object files
	add_executable(demo_fractal src/demo/fractal.cpp)
	add_executable(demo_cam src/demo/cam.cpp)
 
	# Linking executables
	target_link_libraries(demo_cam visiongl ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENCV_LIBRARIES})
	target_link_libraries(demo_fractal visiongl ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENCV_LIBRARIES})
	

# Gererate Makefile for Linux
else()

	# Looking for lib and include folders of glew, glut, opencv and opengl
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (MyOpenCV REQUIRED)
	find_package (OpenGL REQUIRED)

	# Adding folders containing libs
	link_directories(${OPENCV_LIBRARY})

	# Adding folders containing header files
	INCLUDE_DIRECTORIES( src/ ${GLEW_INCLUDE_PATH} ${FREEGLUT_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR} )

	#cria o .object do visiongl, em alguns compiladores gera também a visiongl.lib
	add_library(visiongl SHARED ${visiongl_src} ${FREEGLUT_INCLUDE_DIR} ${GLEW_LIBRARY} ${OPENGL_GLU_LIBRARY} ${OPENCV_LIBRARIES})
        SET_TARGET_PROPERTIES(visiongl PROPERTIES PREFIX "lib/lib")

	# Creating demo_fractal object files
	add_executable(demo_fractal src/demo/fractal.cpp)
        SET_TARGET_PROPERTIES(demo_fractal PROPERTIES PREFIX "bin/")

	# Creating demo_cam object files
	add_executable(demo_cam src/demo/cam.cpp)
        SET_TARGET_PROPERTIES(demo_cam PROPERTIES PREFIX "bin/")
	
	# Linking executables
	target_link_libraries(demo_fractal visiongl ${OPENCV_LIBRARIES}  ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})
	target_link_libraries(demo_cam     visiongl ${OPENCV_LIBRARIES}  ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})

endif()
