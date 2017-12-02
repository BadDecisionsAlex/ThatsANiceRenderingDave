SET(pwd ${CMAKE_CURRENT_LIST_DIR})

SET(src "" OpenGLUtil.cpp OpenGLUtil.h)
AUX_SOURCE_DIRECTORY(${pwd} src)
add_executable(particles ${src})
message(STATUS "particles added ${src}")

target_link_libraries(particles ${stdgl_libraries})
