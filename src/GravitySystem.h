//
//  GravitySystem.h
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef GravitySystem_h
#define GravitySystem_h
#include "ParticleSystem.h"
#include "ParticleGrid.h"

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


#endif /* GravitySystem_h */
