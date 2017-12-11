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

//#include "GridSystemHandler.h"

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec1;

int window_width = 640, window_height = 640;

int main(int argc, char* argv[])
{
    OpenGLUtil openGL = OpenGLUtil(window_width, window_height, "Particles");
    GLFWwindow* window = openGL.setup();
    GUI gui(window);
    
    // Params in Order : 
    // (int) Number of Cells (N+2 x N+2) because of border
    // (int) dx Do Not Change
    // (int) dy Do Not Change
    // (float) dt Timestep (1.0f/60.0f) matches 60 fps
    // (float) diffusion (0.0f-1.0f) density spread rate.
    // (float) viscocity (0.0f-1.0f) velocity spread rate.
    FluidSystem* rootSystem = new FluidSystem( 
            64,     // N size 
            10, 10, (1.0f/60.0f), // don't touch
            0.02f,    // diffusion 
            0.4f     // viscocity
            );

    rootSystem->width = window_width;
    rootSystem->height = window_height;
    rootSystem->setup();
    rootSystem->prepareDraw();
    gui.delegates.push_back(rootSystem);

	while (openGL.drawBool()) {
        openGL.beforeDraw();
        rootSystem->step();
        rootSystem->draw();
        openGL.afterDraw();
	}
    openGL.destroy();
}
