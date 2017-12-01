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

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec1;

const char* particle_vertex_shader =
#include "shaders/particle.vert"
;

const char* particle_geometry_shader =
#include "shaders/particle.geom"
;

const char* particle_fragment_shader =
#include "shaders/particle.frag"
;

int window_width = 1000, window_height = 1000;
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
    
    vector<vec4> points;
    vector<uvec1> point_numbers;

    //Setup gravity system
    vector<VerletParticle> particle_inits;
    particle_inits.push_back(VerletParticle(225.0, 250.0));
    particle_inits.push_back(VerletParticle(275.0, 250.0));
    particle_inits[0].v0 = vec3(0.1, 0.0, 0.0);
    particle_inits[1].v0 = vec3(-0.1, 0.0, 0.0);
    
    particle_inits.push_back(VerletParticle(250, 350));
    particle_inits.push_back(VerletParticle(250, 200));

    // Initialize a Gravity System and Scene
    GravitySystem* rootSystem = new GravitySystem(particle_inits);
    rootSystem->width = window_width;
    rootSystem->height = window_height;
//    rootSystem->prepareDraw();
    rootSystem->getPointsForScreen(points, point_numbers);
    
    RenderDataInput particle_pass_input;
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
    particle_pass_input.assign_index(point_numbers.data(), point_numbers.size(), 1);
    RenderPass particle_pass(-1,
                             particle_pass_input,
                             {
                                 particle_vertex_shader,
                                 particle_geometry_shader,
                                 particle_fragment_shader
                             },
                             { /* uniforms */ },
                             { "fragment_color" }
                             );
    
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
        //glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glCullFace(GL_BACK);
        
        // Make our updates to physics and scene.
        ++counter;
        if (counter % 10 == 0) {
            VerletParticle newParticle( distribution(generator), distribution(generator) );
            rootSystem->particles.push_back(newParticle);
//            rootSystem->step();
//            rootSystem->prepareDraw();
            
            rootSystem->getPointsForScreen(points, point_numbers);
            
            particle_pass_input.assign_index(point_numbers.data(), point_numbers.size(), 1);
            particle_pass = RenderPass(-1,
                                       particle_pass_input,
                                       {
                                           particle_vertex_shader,
                                           particle_geometry_shader,
                                           particle_fragment_shader
                                       },
                                       { /* uniforms */ },
                                       { "fragment_color" }
                                       );
        }else{
            rootSystem->step();
            rootSystem->getPointsForScreen(points, point_numbers);
        }

        //TODO: Draw here
//        rootSystem->prepareDraw();
//        rootSystem->draw();
        
        particle_pass.updateVBO(0, points.data(), points.size());
        
        particle_pass.setup();
        CHECK_GL_ERROR(glDrawElements(GL_POINTS, point_numbers.size(), GL_UNSIGNED_INT, 0));
        
		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
    
	exit(EXIT_SUCCESS);
}
