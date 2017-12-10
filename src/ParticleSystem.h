#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <glm/glm.hpp>          // vec2, vec3
#include <glm/gtc/vec1.hpp>
#include <vector>               // vector
#include <time.h>               // needed for timing the step
#include "AbstractParticle.h"   // AbstractParticle
#include "Drawable.h"
#include "GUIDelegate.h"

using glm::uvec1;
using glm::uvec3;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::vector;
using std::pair;

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON 0.00000001f
#endif

//TODO: make drawable once that is fixed
struct ParticleSystem: Drawable, GUIDelegate {
    float width, height;
    
    ParticleSystem() : width(1000), height(1000) {}
    ParticleSystem(float _w, float _h) : width(_w), height(_h) {}
    
    void virtual step() = 0;
    void virtual setup() {}
    
};

#endif
