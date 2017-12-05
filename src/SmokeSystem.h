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
    
    SmokeSystem();
    
    void setup();
    void step();
    void getPointsForScreen(vector<vec4>& points, vector<vec4>& velocities, vector<uvec1>& indices);
    vec4 toScreen(const vec3& point, int id);
    
    void prepareDraw();
    void draw();
    
private:
    vec3 center;
    int stepCount = 0;
    
    //Rendering (Could be made simpler)
    RenderDataInput particle_pass_input;
    RenderPass particle_pass;
    vector<vec4> points;
    vector<vec4> velocities;
    vector<uvec1> indices;
};

#endif /* SmokeSystem_h */
