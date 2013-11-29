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
	find_package (MyOpenCV REQUIRED)

	#adiciona as pastas de dependência de lib
	link_directories(${OPENCV_LIBRARY})

	#adiciona as pastas de dependência de headers
	INCLUDE_DIRECTORIES( src/ ${OPENCV_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREEGLUT_INCLUDE_DIR})

	#cria o .object do visiongl, em alguns compiladores gera também a visiongl.lib
	add_library(visiongl ${visiongl_src})

	#cria um executável a partir do fractal.cpp
	add_executable(visiongl_fractal src/demo/fractal.cpp)
	add_executable(visiongl_cam src/demo/cam.cpp)
 
	#linka o executável, com as libs
	target_link_libraries(visiongl_cam visiongl ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENCV_LIBRARIES})
	target_link_libraries(visiongl_fractal visiongl ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENCV_LIBRARIES})
	

else()

	#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

	#Procura pelas pastas lib e include dos packages glew, glut, opencv
	find_package (MyGLEW REQUIRED)
	find_package (MyFREEGLUT REQUIRED)
	find_package (MyOpenCV REQUIRED)
	find_package (OpenGL REQUIRED)

	#adiciona as pastas de dependência de lib
	link_directories(${OPENCV_LIBRARY})

	#adiciona as pastas de dependência de headers
	INCLUDE_DIRECTORIES( src/ ${GLEW_INCLUDE_PATH} ${FREEGLUT_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR} )

	#cria o .object do visiongl, em alguns compiladores gera também a visiongl.lib
	add_library(visiongl SHARED ${visiongl_src} ${FREEGLUT_INCLUDE_DIR} ${GLEW_LIBRARY} ${OPENGL_GLU_LIBRARY} ${OPENCV_LIBRARIES})
        SET_TARGET_PROPERTIES(visiongl PROPERTIES PREFIX "lib/lib")

	#cria um executável a partir do fractal.cpp
	add_executable(demo_fractal src/demo/fractal.cpp)
        SET_TARGET_PROPERTIES(demo_fractal PROPERTIES PREFIX "bin/")

	#cria um executável a partir do cam.cpp
	add_executable(demo_cam src/demo/cam.cpp)
        SET_TARGET_PROPERTIES(demo_cam PROPERTIES PREFIX "bin/")
	
	
	#linka o executavel com as libs no linux ou outro sistema
        message(${FREEGLUT_LIBRARY})
        message(${GLEW_LIBRARY})
        message(${OPENGL_glu_LIBRARY})
	target_link_libraries(demo_fractal visiongl ${OPENCV_LIBRARIES}  ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})

	target_link_libraries(demo_cam     visiongl ${OPENCV_LIBRARIES}  ${FREEGLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_glu_LIBRARY})

endif()
