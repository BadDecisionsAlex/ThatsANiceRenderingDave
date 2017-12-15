//
//  SystemHandler.h
//  Particles
//
//  Created by Matthew Ruston on 12/14/17.
//

#ifndef SystemHandler_h
#define SystemHandler_h

#include "GUIDelegate.h"
#include "ParticleSystem.h"

#include "Fluids.h"
#include "SpaceSystem.h"
#include "RainbowSpaceSystem.h"
#include "SmokeSystem.h"
#include "GravitySystem.h"
#include "GridSystemHandler.h"
#include "PointGridSystemHandler.h"

struct SystemHandler: public GUIDelegate {
    
    ParticleSystem *system;
    
    float window_width = 0;
    float window_height = 0;
    
    SystemHandler(float height, float width) : window_width(width), window_height(height) {
        system = makeFluidSystem();
        setUpSystem();
    }
    
    void keyWasPressed( int action, int key ) {
        
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_1) {
                free(system);
                system = makeFluidSystem();
                setUpSystem();
                return;
            } else if (key == GLFW_KEY_2) {
                free(system);
                system = new SpaceSystem();
                setUpSystem(4);
                return;
            } else if (key == GLFW_KEY_3) {
                free(system);
                system = new RainbowSpaceSystem();
                setUpSystem();
                return;
            } else if (key == GLFW_KEY_4) {
                free(system);
                system = new GravitySystem();
                setUpSystem();
                return;
            } else if (key == GLFW_KEY_5) {
                free(system);
                system = new SmokeSystem(false);
                setUpSystem(2);
                return;
            } else if (key == GLFW_KEY_6) {
                free(system);
                system = new SmokeSystem(true);
                setUpSystem(2);
                return;
            } else if (key == GLFW_KEY_7) {
                free(system);
                SpaceSystem* spaceSystem = new SpaceSystem();
                system = new GridSystemHandler(spaceSystem);
                setUpSystem(4);
                return;
            } else if (key == GLFW_KEY_8) {
                free(system);
                SpaceSystem* spaceSystem = new SpaceSystem();
                system = new PointGridSystemHandler(spaceSystem);
                setUpSystem(5);
                return;
            }
        }
        
        
        system->keyWasPressed(action, key);
    }
    
    void mouseDragged(float x, float y) {
        system->mouseDragged(x, y);
    }
    
    void mouseStateChange(bool dragging) {
        system->mouseStateChange(dragging);
    }
    
    void mouseButton(int button) {
        system->mouseButton(button);
    }
    
    void step() {
        system->step();
        system->draw();
    }
    
private:
    FluidSystem* makeFluidSystem() {
        // Params in Order :
        // (int) Number of Cells (N+2 x N+2) because of border
        // (float) dt Timestep (1.0f/60.0f) matches 60 fps
        // (float) diffusion (0.0f-1.0f) density spread rate.
        // (float) viscocity (0.0f-1.0f) velocity spread rate.
        FluidSystem *fluidSystem = new FluidSystem(
                        64,     // N size
                        (1.0f/60.0f), // don't touch
                        0.0013f,    // diffusion
                        0.18f     // viscocity
                        );
        return fluidSystem;
    }
    
    void setUpSystem(int scale = 1) {
        system->width = window_width * scale;
        system->height = window_height * scale;
        system->setup();
        system->prepareDraw();
    }
};

#endif /* SystemHandler_h */
