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

#include "SmokeSystem.h"
#include "MassParticle.h"

#include "GridSystemHandler.h"

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

    FluidSystem* rootSystem = new FluidSystem();
    
    rootSystem->width = window_width;
    rootSystem->height = window_height;
    rootSystem->setup();
    rootSystem->prepareDraw();

    
    //rootSystem->test();

	while (openGL.drawBool()) {
        
        openGL.beforeDraw();

        rootSystem->step();
        rootSystem->draw();

        openGL.afterDraw();
	}
    openGL.destroy();
}
