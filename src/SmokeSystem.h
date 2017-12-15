//
//  SmokeSystem.h
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef SmokeSystem_h
#define SmokeSystem_h
#include "ParticleSystem.h"
#include "MassParticle.h"

class SmokeSystem: public ParticleSystem {
public:
    vector<MassParticle> particles;
    
    SmokeSystem(bool emitter);
    
    void setup();
    void step();
    void getPointsForScreen(vector<vec4>& points, vector<vec4>& velocities, vector<float>& types, vector<uvec1>& indices);
    vec4 toScreen(const vec3& point, int id);
    
    void prepareDraw();
    void draw();
    
    void mouseDragged(float x, float y) {
        mouse = vec3(x * 2, y * 2, 0);
    }
    
    void mouseStateChange(bool dragging) {
        isDragging = dragging;
    }
    
private:
    vec3 center;
    vec3 mouse;
    bool isDragging = false;
    int stepCount = 0;
    bool EMITTER;
    
    //Rendering (Could be made simpler)
    RenderDataInput particle_pass_input;
    RenderPass particle_pass;
    vector<vec4> points;
    vector<vec4> velocities;
    vector<uvec1> indices;
    vector<float> types;
};

#endif /* SmokeSystem_h */
