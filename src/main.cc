#include <GL/glew.h>
#include <dirent.h>

#include "render_pass.h"
#include "gui.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <random>
#include <thread>
#include <chrono>

#include <GLFW/glfw3.h>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

#include "AbstractParticle.h"
#include "ParticleSystem.h"

#include "VerletParticle.h"
#include "GravitySystem.h"

#include "SpaceSystem.h"
#include "SmokeSystem.h"
#include "MassParticle.h"

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec1;

int window_width = 700, window_height = 700;
const std::string window_title = "Particles";

// FIXME: Add more shaders here.

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	//std::cout << "Renderer: " << renderer << "\n";
	//std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

int main(int argc, char* argv[])
{
	GLFWwindow *window = init_glefw();
    GUI gui(window);
    
    vector<ParticleSystem*> systems;
    
    SpaceSystem* particleSystem = new SpaceSystem();
    particleSystem->width = window_width * 8;
    particleSystem->height = window_height * 8;
    particleSystem->setup();
    particleSystem->prepareDraw();
    systems.push_back(particleSystem);
    
    SmokeSystem* rootSystem = new SmokeSystem();
    rootSystem->width = window_width * 2;
    rootSystem->height = window_height * 2;
    rootSystem->setup();
    rootSystem->prepareDraw();
    systems.push_back(rootSystem);
    gui.delegates.push_back(rootSystem);
    
    // **************
    //
    // ANIMATION LOOP
    //
    // **************

    std::default_random_engine generator;
    std::normal_distribution<float> distribution( 250, 60 );
    long counter = 1;

	while (!glfwWindowShouldClose(window)) {
        // THREAD IS SLEEPING!
        // std::this_thread::sleep_for(std::chrono::seconds(1));

        glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glCullFace(GL_BACK);
        
        
        for (ParticleSystem* system : systems) {
            //Step our systems
            system->step();
            
            //TODO: Draw here
            system->draw();
        }
        
		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
