# Directories
LINK_DIRECTORIES("/usr/local/lib" "/opt/local/lib")
INCLUDE_DIRECTORIES("/usr/local/include" "/opt/local/include")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Flags
#set(CMAKE_CXX_FLAGS "--std=c++14 -g -fmax-errors=1")
set(CMAKE_CXX_FLAGS "--std=c++14 -g")

# Packages
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIRS})
LINK_DIRECTORIES(${OPENGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OPENGL_DEFINITIONS})

MESSAGE(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
LIST(APPEND stdgl_libraries ${OPENGL_gl_LIBRARY})

if (APPLE)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa REQUIRED)
endif(APPLE)
