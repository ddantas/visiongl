cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(itk_benchmark)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#include("C:/Users/H_DANILO/Documents/GitHub/visiongl/src/demo/timer.h")

find_package(GDCM REQUIRED)


add_executable(MyTest itk_benchmark.cpp ../timer.cpp)
target_link_libraries(MyTest  ${ITK_LIBRARIES}) #ITKCommon ITKIOImageBase ITKIOJPEG ITKIOPNG ITKIONIFTI ITKIONRRD ITKIOGIPL ITKIOHDF5 ITKIOBMP ITKIOLSM ITKIOVTK ITKIOStimulate ITKIOBioRad ITKIOMeta ITKIOMRC ITKIOGDCM ITKGPUThresholding ITKGPUCommon)

message (${ITK_USE_FILE}) 
