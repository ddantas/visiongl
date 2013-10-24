
#
# makefile for visiongl
#
# ddantas 5/7/2013
#

BINARY_NAME        = visiongl
CUDA_NAME          = vglcuda
FRACTAL_NAME       = fractal
CAM_NAME           = cam

ROOTDIR            = .
INCLUDE_DIR        = $(ROOTDIR)/src

OUTPUT_ROOTDIR     = $(ROOTDIR)
OUTPUT_LIBDIR      = $(OUTPUT_ROOTDIR)/lib
OUTPUT_INCLUDEDIR  = $(OUTPUT_ROOTDIR)/include
OUTPUT_BINDIR      = $(OUTPUT_ROOTDIR)/bin


OPENCV_PATH       = $(HOME)/bin/opencv
OPENCV_INCLUDEDIR = $(OPENCV_PATH)/include/
OPENCV_LIBDIR     = $(OPENCV_PATH)/lib
OPENCV_LIBRARIES  = -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_legacy

CUDA_PATH       = /usr/local/cuda
CUDA_INCLUDEDIR = $(CUDA_PATH)/include
CUDA_LIBDIR     = $(CUDA_PATH)/lib64
CUDA_LIBRARIES  = -lcudart

INSTALL_PATH       = $(HOME)/script
INSTALL_INCLUDEDIR = $(INSTALL_PATH)/include
INSTALL_LIBDIR     = $(INSTALL_PATH)/lib

OPENGL_LIBDIR = /usr/X11R6/lib

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(OUTPUT_LIBDIR):$(CUDA_LIBDIR):$(OPENCV_LIBDIR):$(INSTALL_LIBDIR)

COMPILER_FLAGS   = -g -pg -DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES 
OPENGL_LIBRARIES    = -lGL -lGLU -lglut 

CC               = g++
NVCC             = $(CUDA_PATH)/bin/nvcc

LINUXAMD64_DIRS_LIBS =  -I $(INCLUDE_DIR) \
                        -L $(OUTPUT_LIBDIR) \
                        -L $(OPENGL_LIBDIR) \
                           $(OPENGL_LIBRARIES) \
                        -I $(OPENCV_INCLUDEDIR) \
                        -L $(OPENCV_LIBDIR) \
                           $(OPENCV_LIBRARIES) \
                        -I $(CUDA_INCLUDEDIR) \
                        -L $(CUDA_LIBDIR) \
                           $(CUDA_LIBRARIES)\


LINUXAMD64_LIB = $(NVCC) $(COMPILER_FLAGS) \
                        -shared -Xcompiler -fPIC \
                        -o $(OUTPUT_LIBDIR)/lib$(BINARY_NAME).so \
                          src/*.cu src/*.cpp \
                         $(LINUXAMD64_DIRS_LIBS) $(CUDA_DIRS_LIBS) 

LINUXAMD64_DEMO_FRACTAL = $(NVCC) $(COMPILER_FLAGS) \
                        -o $(OUTPUT_BINDIR)/demo_$(FRACTAL_NAME) \
                         src/demo/$(FRACTAL_NAME).cpp \
                         -lvisiongl \
                         $(LINUXAMD64_DIRS_LIBS)

LINUXAMD64_DEMO_CAM = $(CC) $(COMPILER_FLAGS) \
                        -o $(OUTPUT_BINDIR)/demo_$(CAM_NAME) \
                         src/demo/$(CAM_NAME).cpp \
                         -lvisiongl \
                         $(LINUXAMD64_DIRS_LIBS)



GENERATE_HEADER = cat $(INCLUDE_DIR)/vglHead.h $(INCLUDE_DIR)/vglImage.h $(INCLUDE_DIR)/vglCudaImage.h $(INCLUDE_DIR)/vglContext.h $(INCLUDE_DIR)/vglSimpleBGModel.h $(INCLUDE_DIR)/glsl2cpp*.h $(INCLUDE_DIR)/kernel2cu*.h $(INCLUDE_DIR)/vglTail.h > /tmp/$(BINARY_NAME).h; grep -v vglImage\.h /tmp/$(BINARY_NAME).h > $(OUTPUT_INCLUDEDIR)/$(BINARY_NAME).h


all:
	$(GENERATE_HEADER)
	$(LINUXAMD64_LIB)

lib:
	$(LINUXAMD64_LIB)

install: all
	cp $(OUTPUT_INCLUDEDIR)/$(BINARY_NAME).h $(INSTALL_INCLUDEDIR)
	cp $(OUTPUT_LIBDIR)/lib$(BINARY_NAME).so $(INSTALL_LIBDIR)

dox: all
	doxygen $(BINARY_NAME).dox
	cd dox/latex; pwd; make; xdvi refman.dvi&

demofrac:
	$(LINUXAMD64_DEMO_FRACTAL)

democam:
	$(LINUXAMD64_DEMO_CAM)

.PHONY: lib


rundemofrac:
	cd $(OUTPUT_BINDIR); ./demo_$(FRACTAL_NAME)

rundemocam:
	cd $(OUTPUT_BINDIR); ./demo_$(CAM_NAME)

cuda:
	./kernel2cu.pl -o src/kernel2cu_shaders src/CUDA/*.kernel

frag:
	./glsl2cpp.pl -o src/glsl2cpp_shaders -p FS src/FS/*.frag

frag_bg:
	./glsl2cpp.pl -o src/glsl2cpp_BG -p FS_BG src/FS_BG/*.frag

frag_stereo:
	./glsl2cpp.pl -o src/glsl2cpp_Stereo -p FS_Stereo src/FS_Stereo/*.frag

frag_diffeq:
	./glsl2cpp.pl -o src/glsl2cpp_DiffEq -p FS_DiffEq src/FS_DiffEq/*.frag

run:
	cd $(OUTPUT_BINDIR); ./$(BINARY_NAME)

runtestcam:
	cd $(OUTPUT_BINDIR); ./$(TESTCAM_NAME)

debug:
	cd $(OUTPUT_BINDIR); ddd ./$(BINARY_NAME)

bkp:
	rm -f bkp.tgz
	tar -cvzf bkp.tgz *

