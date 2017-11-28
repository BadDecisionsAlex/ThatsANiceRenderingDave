#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <time.h>               // needed for timing the step
#include "ParticleGrid.h"

using glm::vec2;
using glm::vec3;
using std::vector;
using std::pair;

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON 0.00000001f
#endif

struct ParticleSystem {
    float width, height;
    
    ParticleSystem() : width(500.0), height(500.0) {}
    ParticleSystem(float _w, float _h) : width(_w), height(_h) {}
    
    void virtual sendData(vector<vec3>&  points){};
    void virtual step()=0;
};

class GravitySystem : public ParticleSystem {
    const vec3 DEFAULT_GRAVITY = vec3( 0.0, -9.807, 0.0 );
    vector<pair<int,int>> flaggedForCollides;
    public: 
        vector<VerletParticle> particles;
        vec3 gForce = DEFAULT_GRAVITY;
        GravitySystem() : particles(){}
        GravitySystem( float _g) : particles(), gForce( vec3( 0.0, _g, 0.0 ) ){}
        GravitySystem( vec3 _g) : particles(), gForce( _g ){}
        GravitySystem( const vector<vec2>& _in);
        GravitySystem( const vector<VerletParticle>& _in ) : particles( _in ), grid(10.0f, width, height){}
        void sendData( vector<vec3>& points );
        void step();
    private:
        ParticleGrid grid;

        void executeCollisions();
        bool collides( const VerletParticle& lhs, const VerletParticle& rhs );
        void fixCollision( VerletParticle& lsh, VerletParticle& rhs );
        short inBounds( const VerletParticle& _p );
        void fixBounds( VerletParticle& _p, const short& flag );
        void fixBounds( VerletParticle& _p );
};
#endif
