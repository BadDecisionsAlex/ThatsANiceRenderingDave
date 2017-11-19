#ifndef PARTICLE_SYSTEM_CPP
#define PARTICLE_SYSTEM_CPP
#include "ParticleSystem.h"

using glm::vec2;
using glm::vec3;
using std::vector;
using std::make_pair;


GravitySystem::GravitySystem(float _g, const vector<vec2>& _in) {
    for (vec2 _p : _in)
        particles.push_back(VerletParticle(_p));
}

bool GravitySystem::sendData(vector<vec3>& points) {
    points.clear();
    for (VerletParticle& vp : particles)
        points.push_back(vec3(vp.pos(),vp.radius));
    return true;
}

bool GravitySystem::step() {
    flaggedForBounds.clear();
    flaggedForCollides.clear();
    
    // Apply velocity and gravity
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
        vp.p0 += vp.velocity();
        vp.p0 += gForce;
        if(!inBounds(vp))
            flaggedForBounds.push_back(a);
        for (int b = a + 1; b < particles.size(); ++b) {
            if (collides(vp,particles[b]))
                flaggedForCollides.push_back(make_pair(a,b));
        }
    }
    
    // Collision and Bounds corrections
    while (!correctCollides() || !correctBounds());
    
    // Update Particle Data
    for (VerletParticle vp : particles) {
//        vp.p1 = p;
//        vp.p = p0;
    }
    
    return true;
}

vector<int> GravitySystem::flagCollidesFor(const VerletParticle& _p) {
    vector<int> flags;
    
    for (int i = 0; i < particles.size(); ++i) {
        float radiusSum = particles[i].radius +_p.radius + 2 * FLOAT_EPSILON;
        if ((particles[i] != _p) && (glm::distance(particles[i].p0, _p.p0) < radiusSum))
            flags.push_back(i);
    }
    
    return flags;
}

bool GravitySystem::correctCollides() {
    if (flaggedForCollides.size() == 0)
        return true;
    
    for (pair<int, int>& p : flaggedForCollides) {
        VerletParticle& a = particles[p.first];
        VerletParticle& b = particles[p.second];
        
        vector<int> cA = flagCollidesFor(a);
        vector<int> cB = flagCollidesFor(b);
        float wA = cA.size();
        float wB = cB.size();
        vec3 fA = wA*a.acceleration();
        vec3 fB = wB*b.acceleration();
        // FIXME add weight to particles. Weight = 999 if it's touching the bounds.
    }
    
    return true;
}

bool GravitySystem::collides(const VerletParticle& lhs, const VerletParticle& rhs) {
    return true;
}

void GravitySystem::fixCollides(VerletParticle& lsh, VerletParticle& rhs) {
    
}

bool GravitySystem::correctBounds() {
    return true;
}

bool GravitySystem::inBounds(const VerletParticle& _p) {
    return true;
}

void GravitySystem::fixBounds(VerletParticle& _p) {
    
}

#endif
