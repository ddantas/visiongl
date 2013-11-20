#CMAKE que gera automaticamente a lib do visiongl e ja linka com um executável demo
#Por enquanto está linkando apenas com fractal e ja gerando o executável
#Passível de adaptação para gerar qualquer exemplo
#Falta adaptar também para cuda

#É necessario Cmake versão 2.8
cmake_minimum_required(VERSION 2.8)
#O nome do projeto principal É visiongl
project(visiongl)

#visiongl_src é a variável que guarda todos os cpp, ao adicionar ou remover, mexer apenas aqui
set(visiongl_src src/vglImage.cpp src/vglSimpleBGModel.cpp src/vglLoadShader.cpp src/vglContext.cpp src/glsl2cpp_Stereo.cpp src/glsl2cpp_shaders.cpp src/glsl2cpp_DiffEq.cpp src/glsl2cpp_BG.cpp)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")

#verifica se o sistema é windows
if(WIN32)

	#Procura pelas pastas lib e include dos packages glew, glut, opencv
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (OpenCV REQUIRED)

	#depuracao
	#message(${visiongl_src})
	#message("Glew include dir: " ${GLEW_INCLUDE_PATH})
	#message("OpenCV " ${OpenCV_LIBS} ${OpenCV_LIB_DIR} ${OpenCV_INCLUDE_DIRS} )

	#adiciona as pastas de dependência de lib
	link_directories(${opencv_lib_dir})

	#adiciona as pastas de dependência de headers
	INCLUDE_DIRECTORIES( src/ ${opencv_include_dirs})

	#cria o .object do visiongl, em alguns compiladores gera também a visiongl.lib
	add_library(visiongl ${visiongl_src})

	#cria um executável a partir do fractal.cpp
	add_executable(visiongl_fractal src/demo/fractal.cpp)
	add_executable(visiongl_cam src/demo/cam.cpp)
 
	#linka o executável, com as libs
	target_link_libraries(visiongl_cam visiongl ${OpenCV_LIBS} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}/Release/Win32/glew32.lib)
	target_link_libraries(visiongl_fractal visiongl ${OpenCV_LIBS} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}/Release/Win32/glew32.lib)
	INSTALL(TARGETS visiongl visiongl_cam visiongl_fractal
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION bin
			ARCHIVE DESTINATION bin
	)

else()

	set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
	message(${CMAKE_MODULE_PATH})

	#Procura pelas pastas lib e include dos packages glew, glut, opencv
	find_package (GLEW REQUIRED)
	find_package (FREEGLUT REQUIRED)
	find_package (OpenCV REQUIRED)
	find_package (OpenGL REQUIRED)

	#depuracao
	message(${visiongl_src})
	message("Glew include dir: " ${GLEW_INCLUDE_PATH})
	message("OpenCV " ${OpenCV_LIBS} ${OpenCV_LIB_DIR} ${OpenCV_INCLUDE_DIRS} )

	#adiciona as pastas de dependência de lib
	link_directories(${GLEW_LIBRARIES} ${GLUT_LIBRARIES})

	#adiciona as pastas de dependência de headers
	INCLUDE_DIRECTORIES( src/ ${GLEW_INCLUDE_PATH} ${GLUT_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR} )

	#cria o .object do visiongl, em alguns compiladores gera também a visiongl.lib
	add_library(visiongl ${visiongl_src} )

	#cria um executável a partir do fractal.cpp
	add_executable(visiongl_fractal src/demo/fractal.cpp)

	#linka o executavel com as libs no linux ou outro sistema
	target_link_libraries(visiongl_fractal visiongl ${OpenCV_LIBS} ${GLUT_LIBRARIES}  ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})

endif()
