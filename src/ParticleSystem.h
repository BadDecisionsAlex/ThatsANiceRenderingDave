#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <time.h>               // needed for timing the step
#include "AbstractParticle.h"   // AbstractParticle
#include "Drawable.h"

using glm::vec2;
using glm::vec3;
using std::vector;
using std::pair;

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON 0.00000001f
#endif

struct ParticleSystem: Drawable {
    float width, height;
    
    ParticleSystem() : width(1000), height(1000) {}
    ParticleSystem(float _w, float _h) : width(_w), height(_h) {}
    
    void virtual step() = 0;
};

#endif
