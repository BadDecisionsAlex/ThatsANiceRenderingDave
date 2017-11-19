#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
using glm::vec2;
using glm::vec3;
using std::vector;

struct ParticleSystem{
    static const float FLOAT_EPSILON;
    float width, height;
    ParticleSystem():width(500.0), height(500.0){}
    ParticleSystem(float _w, float _h):width(_w), height(_h){}
    bool virtual sendData(vector<vec3>&  points){return false;};
    bool virtual step()=0;
};
const float ParticleSystem::FLOAT_EPSILON=0.00000001;

class GravitySystem : ParticleSystem{
    vector<VerletParticle> particles;
    vec3 gForce;
    public:
        static const vec3 DEFAULT_GRAVITY;
        GravitySystem():particles(), gForce(DEFAULT_GRAVITY){}
        GravitySystem(float _g):particles(), gForce(vec3(0.0,_g,0.0)){}
        GravitySystem(vec3 _g):particles(), gForce(_g){}
        GravitySystem(float _g, const vector<vec2>& _in);
        GravitySystem(float _g, const vector<VerletParticle>& _in);
        bool sendData(vector<vec3>& points);
        bool step();
    private:
        bool correctCollides();
        bool collides(const VerletParticle& lhs, const VerletParticle& rhs);
        void correctCollids(VerletParticle& lsh, VerletParticle& rhs);
        bool correctBounds();
        bool inBounds(const VerletParticle& _p);
        void correctBounds(VerletParticle& _p);
};
const vec3 GravitySystem::DEFAULT_GRAVITY  =  vec3(0.0,-9.807,0.0);
#endif
