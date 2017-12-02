//
// Created by Joshua Cristol on 12/2/17.
//

#ifndef PARTICLES_OPENGLUTIL_H
#define PARTICLES_OPENGLUTIL_H


#include <GLFW/glfw3.h>
#include <string>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <debuggl.h>
#include <GL/glew.h>
#include <iostream>

class OpenGLUtil {
public:
    OpenGLUtil(int window_width, int window_height, const std::string window_title)
            : window_width(window_width), window_height(window_height), window_title(window_title){}
    GLFWwindow* setup();
    void destroy();
    int drawBool();
    void beforeDraw();
    void afterDraw();

private:
    GLFWwindow* window;
    int window_width;
    int window_height;
    const std::string window_title;
};


#endif //PARTICLES_OPENGLUTIL_H
