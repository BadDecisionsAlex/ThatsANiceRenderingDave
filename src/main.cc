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
#include "RainbowSpaceSystem.h"

#include "SystemHandler.h"

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
    
    SystemHandler* systemHandler = new SystemHandler(window_height, window_width);
    gui.delegate = systemHandler;
    
	while (openGL.drawBool()) {
        openGL.beforeDraw();
        
        systemHandler->step();
        
        openGL.afterDraw();
	}
    openGL.destroy();
}
