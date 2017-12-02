#include <GL/glew.h>
#include "render_pass.h"
#include "gui.h"

#include <fstream>
#include <iostream>

#include <random>
#include <thread>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>

#include "ParticleSystem.h"

#include "GravitySystem.h"

#include "SpaceSystem.h"
#include "OpenGLUtil.h"

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec1;

int window_width = 1000, window_height = 1000;


int main(int argc, char* argv[])
{
    OpenGLUtil openGL = OpenGLUtil(window_width, window_height, "Particles");
    GLFWwindow* window = openGL.setup();
    GUI gui(window);

    SpaceSystem* rootSystem = new SpaceSystem();
    rootSystem->width = window_width;
    rootSystem->height = window_height;
    rootSystem->setup();

    //Do initial prepare
    rootSystem->prepareDraw();
	while (openGL.drawBool()) {
        openGL.beforeDraw();
        // Step our systems
        rootSystem->step();
        // call system the draw functions
        rootSystem->draw();
		// Poll and swap.
        openGL.afterDraw();
	}
    openGL.destroy();
}
