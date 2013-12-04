        VisionGL

The objective of this library is to provide an easy way to prototype and 
implement image and video processing functions in CUDA and GLSL. The library
provides wrapper code generators for both CUDA and GLSL languages. 

The wrapper functions are generated in C++ from the shader code, written 
in either CUDA or GLSL. The shader code must contain three types of 
directives in comments:
1 - Documentation comment: a multiple line comment between /** and */
    This comment is copied ipsis literis to the output files.
2 - Declaration comment: a single line comment describing the parameter 
    list of the wrapper function. The list has variable name, type, and, when  
    it is an image, semantics indicating if it is an input or output image.
3 - Attribution comment: single line comments associating variables in C++
    to its counterparts in GLSL or CUDA

The wrapper functions use a common variable type to store image data, called 
VglImage. It contains space to store the image data in RAM, and handles to 
store the image in both CUDA and OpenGL contexts. These contexts are used 
under demand. If the image is used only in OpenGL context, the CUDA handle 
will be empty (NULL or -1) and vice-versa.


        Compilation

The library requires OpenCV 2.4.0 or superior, CUDA 4 or superior, 
GLEW 1.6 or superior and freeglut 3 or superior. OpenGL is also required!

To install GLEW:
apt-get install libglew1.6 libglew1.6-dev

To install freeglut:
apt-get install freeglut3 freeglut3-dev

To install OpenCV:
apt-get install libopencv-dev


        Compilation using cmake
On Linux:
		
Go to the root directory of the project and run:
cmake .
make

On Windows:

Go to the root directory of the project and run:
cmake .

Depending on what generator you have configured it'll create a solution or
a project or a MinGW Makefile. Only tested with Microsoft Visual Studio 2010
as generator. In this case, a solution will be built using the name "visiongl",
just open the solution as usual on Visual Studio, and compile it, you're done.

If you're willing to use Visual Studio 2010 as generator, use the following command:

cmake . -G "Visual Studio 10"


        Compilation using Makefile_linux

Please open Makefile_linux and edit the variables OPENCV_PATH, CUDA_PATH, 
referring to the paths where OpenCV and CUDA are installed. Edit also
INSTALL_PATH, referring to the path where the library and its include files
will be installed.

To build the library:
make -f Makefile_linux all

To install the library to INSTALL_PATH
make -f Makefile_linux install

To build the demos:
make -f Makefile_linux democam
make -f Makefile_linux demofrac

To run the demos:
make -f Makefile_linux rundemocam
make -f Makefile_linux rundemofrac

To build the documentation:
build dox

To generate wrapper code after some shader is modified:
make -f Makefile_linux frag
make -f Makefile_linux frag_bg
make -f Makefile_linux frag_stereo
make -f Makefile_linux frag_diffeq
make -f Makefile_linux cuda


References

[1] Daniel Oliveira Dantas, Junior Barrera
Automatic generation of wrapper code for video processing functions
Learning and Nonlinear models, Vol 9, Num 2, 2011
http://www.deti.ufc.br/~lnlm/papers/vol9-no2-art5.pdf

