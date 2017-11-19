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
        fixBounds(vp);
    }
    
    // Update Particle Data
    for (VerletParticle vp : particles) {
        vp.p1 = vp.p;
        vp.p = vp.p0;
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
    return false;
}

bool GravitySystem::collides(const VerletParticle& lhs, const VerletParticle& rhs) {
    return true;
}

void GravitySystem::fixCollides(VerletParticle& lsh, VerletParticle& rhs) {
    
}

bool GravitySystem::correctBounds() {
    for(int i : flaggedForBounds){
        fixBounds(particles[flaggedForBounds[i]]);
    }
    flaggedForBounds.clear();
    return true;
}

bool GravitySystem::inBounds(const VerletParticle& _p) {
  if(_p.p0.x-_p.radius<FLOAT_EPSILON)
      return false;
  if(_p.p0.y-_p.radius<FLOAT_EPSILON)
      return false;
  if(_p.p0.x+_p.radius>width-FLOAT_EPSILON)
      return false;
  if(_p.p0.y+_p.radius>height-FLOAT_EPSILON)
      return false;

  return true;
}

void GravitySystem::fixBounds(VerletParticle& _p) {
  if(_p.p0.x-_p.radius<FLOAT_EPSILON)
      _p.p0.x = _p.radius;
  if(_p.p0.y-_p.radius<FLOAT_EPSILON)
      _p.p0.y = _p.radius;
  if(_p.p0.x+_p.radius>width-FLOAT_EPSILON)
      _p.p0.x = width - _p.radius;
  if(_p.p0.y+_p.radius>height-FLOAT_EPSILON)
      _p.p0.y = height - _p.radius;
}

#endif
