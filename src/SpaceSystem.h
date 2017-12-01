//
//  SpaceSystem.h
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef SpaceSystem_h
#define SpaceSystem_h
#include "ParticleSystem.h"
#include "MassParticle.h"

class SpaceSystem: public ParticleSystem {
public:
    vector<MassParticle> particles;
    
    SpaceSystem() {}
    
    void step();
    void getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices);
    vec4 toScreen(const vec3& point);
};

#endif /* SpaceSystem_h */
