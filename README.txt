        VisionGL

The objective of this library is to provide an easy way to prototype and 
implement image and video processing functions in CUDA, GLSL and OpenCL. 
The library provides wrapper code generators for all the three languages.

The wrapper functions are generated in C++ from the shader code. Shader 
codes written in CUDA or GLSL must contain three types of directives in 
comments:
1 - Documentation comment: a multiple line comment between /** and */
    This comment is copied ipsis literis to the output files.
2 - Declaration comment: a single line comment describing the parameter 
    list of the wrapper function. The list has variable name, type, and, when  
    it is an image, semantics indicating if it is an input or output image.
3 - Attribution comment: single line comments associating variables in C++
    to its counterparts in GLSL or CUDA

Shader code written in OpenCL require only the two first kinds of comments.

The wrapper functions use a common variable type to store image data, called 
VglImage. It contains space to store the image data in RAM, and handlers to 
store the image in CUDA, OpenGL and OpenCL contexts. These contexts 
are used under demand. If the image is used only in OpenGL context, the 
CUDA handler will be empty (NULL or -1) and vice-versa.


        Compilation

The library requires GLEW 1.6 or superior and freeglut 3 or superior. 
OpenGL support by the hardware is also required!

To install GLEW:
apt-get install libglew1.6 libglew1.6-dev

To install freeglut:
apt-get install freeglut3 freeglut3-dev

To install OpenCV:
apt-get install libopencv-dev

The library can optionally use CUDA 4 or superior, OpenCL, OpenCV, 
libTiff, GDCM or DCMTK.


        Compilation using cmake
On Linux:
		
Go to the root directory of the project and run:
cmake .
make

On Windows:

Go to the root directory of the project and run:
cmake .

Depending on the compiler you have installed, cmake will create a solution,
a project or a MinGW Makefile. The generator works with Microsoft Visual 
Studio 2010. In this case, a solution will be built using the name "visiongl",
just open the solution as usual on Visual Studio, and compile it, you're done.

If you want to use Visual Studio 2010, use the following command:

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
make -f Makefile_linux demobenchmark
make -f Makefile_linux demoio
make -f Makefile_linux demotiff
make -f Makefile_linux demogdcm
make -f Makefile_linux demodcmtk
make -f Makefile_linux demobin

To run the demos:
make -f Makefile_linux rundemocam
make -f Makefile_linux rundemofrac
make -f Makefile_linux rundemobenchmark
make -f Makefile_linux rundemoio
make -f Makefile_linux rundemotiff
make -f Makefile_linux rundemogdcm
make -f Makefile_linux rundemodcmtk
make -f Makefile_linux rundemobin

To build the documentation:
build dox

To generate wrapper code after some shader is modified:
make -f Makefile_linux cl
make -f Makefile_linux frag
make -f Makefile_linux frag_bg
make -f Makefile_linux frag_stereo
make -f Makefile_linux frag_diffeq
make -f Makefile_linux cuda

        Compilation with OpenCV

Please download OpenCV 2.4.0 or superior. It is available from:
https://github.com/opencv/opencv 

To compile VisionGL with support to OpenCV, please set WITH_OPENCV = 1 in 
the VisionGL makefile.

        Compilation with libTIFF

Please install libtiff 4.0 or superior. Install it by running:
sudo apt-get install libtiff4 libtiff4-dev

To compile VisionGL with support to libTIFF, please set WITH_TIFF = 1 in 
the VisionGL makefile.

        Compilation with GDCM

Please download gdcm-2.4.2 source code or later. It is available in: 
http://sourceforge.net/projects/gdcm/files/gdcm%202.x/GDCM%202.4.2/

Set the options below as follows:
GDCM_BUILD_SHARED_LIBS: ON
CMAKE_INSTALL_PREFFIX: /usr/local/gdcm

Optionally, set also
GDCM_BUILD_APPLICATIONS: ON

Then run:
make
sudo make install

To compile VisionGL with support to GDCM, please set WITH_GDCM = 1 in 
the VisionGL makefile.

        Compilation with DCMTK

Please download dcmtk-3.6.5.zip source code or later, available in:
https://dicom.offis.de/download/dcmtk/dcmtk365/

Set the options below as follows:
BUILD_SHARED_LIBS: ON
CMAKE_INSTALL_PREFFIX: /usr/local/dcmtk

Then run:
make
sudo make install

To compile VisionGL with support to DCMTK, please set WITH_DCMTK = 1 in 
the VisionGL makefile.

        Compilation with OpenCL

Please download AMD-APP-SDK-v2.9.1-lnx64.tar.xz, available in:
https://github.com/ghostlander/AMD-APP-SDK/releases

Then run:
tar -xf AMD-APP-SDK-v2.9.1-lnx64.tar.xz
cd AMD-APP-SDK-v2.9.1-lnx64
sudo ./install.sh

The install path may be left as /opt.

To compile VisionGL with support to OpenCL, please set WITH_OPENCL = 1 in
the VisionGL makefile.

        About the demos

demofrac: Probably the most basic demo. Does not require any aditional 
library besides the minimum. Use the arrows in the numpad to navigate,
 z/x to zoom in/out, and q to quit. 
 
democam: Another basic demo. Requires OpenCV to obtain images from the 
webcam. Captures images from the first webcam found. The capture is done by 
OpenCV function cvCaptureFromCAM. An OpenGL window shows four versions 
of the captured image after a few operations.
 
demoio and demotiff: To run these examples, please compile VisionGL with
TIFF support.

demogdcm: To run this example, please compile VisionGL with
GDCM support. In a previous section there are instructions about how
to compile GDCM.

demodcmtk: To run this example, please compile VisionGL with
DCMTK support. In a previous section there are instructions about how
to compile DCMTK.

demobenchmark: This demo is composed by six programs.
 - demobenchmark_cv: The simplest one, requires OpenCV.
 - demobenchmark_cl: To run this example, please compile VisionGL with
                     OpenCL support.
 - demobenchmark_cvocl: To run this example, please compile OpenCV with
                     OpenCL support. Download OpenCV and look for the 
                     folder modules/ocl. It may be not present in the
                     master branch from the git repository, but it is 
                     present in the 2.4.8 and 2.4.9 tags. Get OpenCV
                     with git by running:

                     git clone https://github.com/opencv/opencv.git
                     git checkout 2.4.9

 - demobenchmark_cl3d: To run this example, please compile VisionGL with
                     OpenCL support.
                     Your device must support the extension
                     cl_khr_3d_image_writes. This extension is available 
                     in most AMD video cards. Please use the function 
                     clGetDeviceInfo with the option CL_DEVICE_EXTENSIONS
                     to check the extension availability. An alternative is 
                     to look for the card model in compupench.org and look 
                     for the card details.

 - demobenchmark_mm: To run this example, please compile VisionGL with
                     OpenCL support.

 - demobenchmark_fuzzytophat: To run this example, please compile VisionGL with
                     OpenCL support.

demobenchmark_clnd: To run this example, please compile VisionGL with
OpenCL support.

demobin: Examples of operations on binary images (1bpp). To run this 
example, please compile VisionGL with OpenCL support.

References


[1] Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa
Fast 2D and 3D image processing with OpenCL
International Conference on Image Processing (ICIP), 2015, IEEE, Quebec City.
http://dx.doi.org/10.1109/ICIP.2015.7351730
http://ieeexplore.ieee.org/xpl/articleDetails.jsp?reload=true&arnumber=7351730

[2] Daniel Oliveira Dantas, Helton Danilo Passos Leal, Davy Oliveira Barros Sousa
Fast multidimensional image processing with OpenCL
International Conference on Image Processing (ICIP), 2016, IEEE, Phoenix City.
http://doi.org/10.1109/ICIP.2016.7532664
http://ieeexplore.ieee.org/document/7532664/

[3] Daniel Oliveira Dantas, Junior Barrera
Automatic generation of wrapper code for video processing functions
Learning and Nonlinear models, Vol 9, Num 2, 2011
https://www.academia.edu/4367451/Automatic_generation_of_wrapper_code_for_video_processing_functions
http://www.deti.ufc.br/~lnlm/papers/vol9-no2-art5.pdf

