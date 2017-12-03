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
#include "Fluids.h"

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

    // create particle systems
//    SpaceSystem* rootSystem = new SpaceSystem();
//    rootSystem->width = window_width;
//    rootSystem->height = window_height;
//    rootSystem->setup();
//
//    rootSystem->prepareDraw();

    FluidSystem* f = new FluidSystem(100, 10, 10, (1.0f/ 60.0f));

	while (openGL.drawBool()) {
        openGL.beforeDraw();
        // Step our systems
//        rootSystem->step();
        // call system the draw functions
//        rootSystem->draw();
        openGL.afterDraw();
	}
    openGL.destroy();
}
