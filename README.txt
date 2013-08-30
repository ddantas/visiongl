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

The library requires OpenCV 2.5 or superior, CUDA 4 or superior, and 
freeglut 3 or superior.

Please open the Makefile and edit the variables OPENCV_PATH, CUDA_PATH, 
referring to the paths where OpenCV and CUDA are installed. Edit also
INSTALL_PATH, referring to the path where the library and its include files
will be installed.

To build the library:
make all

To install the library to INSTALL_PATH
make install

To build the demos:
make democam
make demofrac

To run the demos:
make rundemocam
make rundemofrac

To build the documentation:
build dox

To generate wrapper code after some shader is modified:
make frag
make frag_bg
make frag_stereo
make frag_diffeq
make cuda


References

[1] Daniel Oliveira Dantas, Junior Barrera
Automatic generation of wrapper code for video processing functions
Learning and Nonlinear models, Vol 9, Num 2, 2011
http://www.deti.ufc.br/~lnlm/papers/vol9-no2-art5.pdf

