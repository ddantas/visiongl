# - Try to find GLEW
# Once done this will define
#  GLEW_FOUND - System has GLEW
#  GLEW_INCLUDE_DIRS - The GLEW include directories
#  GLEW_LIBRARIES - The libraries needed to use GLEW
#  GLEW_DEFINITIONS - Compiler switches required for using GLEW

find_package(GLEW REQUIRED)
if (!GLEW_FOUND)
{
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	{
		set(possible_paths 
		  "C:/Program Files/Microsoft Visual Studio"
		  "${CSIDL_PROGRAM_FILES}/Microsoft Visual Studio"
		  ${CSIDL_PROGRAM_FILES}
		)
		message("possiveis caminhos: " ${possible_paths})	
	}
}