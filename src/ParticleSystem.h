#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <time.h>               // needed for timing the step

using glm::vec2;
using glm::vec3;
using std::vector;
using std::pair;

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON 0.00000001
#endif

struct ParticleSystem {
    float width, height;
    
    ParticleSystem() : width(500.0), height(500.0) {}
    ParticleSystem(float _w, float _h) : width(_w), height(_h) {}
    
    bool virtual sendData(vector<vec3>&  points){return false;};
    bool virtual step()=0;
};

class GravitySystem : public ParticleSystem {
    public:
        static const vec3 DEFAULT_GRAVITY;
    
        vector<VerletParticle> particles;
        GravitySystem():particles(), gForce(DEFAULT_GRAVITY){}
        GravitySystem(float _g):particles(), gForce(vec3(0.0,_g,0.0)){}
        GravitySystem(vec3 _g):particles(), gForce(_g){}
        GravitySystem(vec3 _g, const vector<vec2>& _in );
        GravitySystem(float _g, const vector<vec2>& _in):GravitySystem( vec3(0.0,_g,0.0), _in ){}
        GravitySystem(const vector<vec2>& _in):GravitySystem(DEFAULT_GRAVITY, _in){}
        GravitySystem(float _g, const vector<VerletParticle>& _in):particles(_in), gForce( vec3( 0.0, _g, 0.0 ) ){}
    
        bool sendData(vector<vec3>& points);
        bool step();

    protected:
        vec3 gForce;
        bool correctCollides();
        bool collides(const VerletParticle& lhs, const VerletParticle& rhs);
        void fixBounds(VerletParticle& _p);
};

// add a class that if inherited will handle collisions in a grid

class GriddedGravitySystem : public GravitySystem {
public:
    typedef std::vector<VerletParticle> VerletVector;
    typedef std::vector<VerletVector> ColVector;
    typedef std::vector<ColVector> ParticleVector;


    GriddedGravitySystem(const vector<vec2> &_in) : GravitySystem(_in) { gridInit(10); }

    bool step();

protected:
    ParticleVector grid;
    int blockSize;
    int dividedWidth;
    int dividedHeight;
    bool correctCollides();

    void gridInit(int blockSize);
    void gridInsert(VerletParticle p);
    vector<VerletParticle> findCollisions(VerletParticle p);
    vector<VerletParticle> getCell(VerletParticle p);
};
#endif
