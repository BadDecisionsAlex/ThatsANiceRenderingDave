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

void stdSetup(ParticleSystem& p){
    p.width = window_width;
    p.height = window_height;
    p.setup();
    p.prepareDraw();
}

int main(int argc, char* argv[])
{
    OpenGLUtil openGL = OpenGLUtil(window_width, window_height, "Particles");
    GLFWwindow* window = openGL.setup();
    GUI gui(window);
    vector<ParticleSystem> systems;
    
    // Params in Order : 
    // (int) Number of Cells (N+2 x N+2) because of border
    // (float) dt Timestep (1.0f/60.0f) matches 60 fps
    // (float) diffusion (0.0f-1.0f) density spread rate.
    // (float) viscocity (0.0f-1.0f) velocity spread rate.
    FluidSystem rootSystem(
            64,     // N size 
            (1.0f/60.0f), // don't touch
            0.0013f,    // diffusion 
            0.18f     // viscocity
            );

    stdSetup(rootSystem);
    gui.delegates.push_back(&rootSystem);
    systems.push_back(rootSystem);

	while (openGL.drawBool()) {
        openGL.beforeDraw();
        for (int i = 0; i < systems.size(); ++i) {
           systems[i].step();
            systems[i].draw();
        }
        openGL.afterDraw();
	}
    openGL.destroy();
}
