#ifndef PARTICLE_SYSTEM_CPP
#define PARTICLE_SYSTEM_CPP
#include "ParticleSystem.h"
#include <iostream>                 // std::cout
#include <glm/gtx/string_cast.hpp>  // glm::to_string
#include <glm/glm.hpp>              // glm::normalize

#define STEP_TIME 1.0f / 60.0f

// To disable debug messages for this file comment out the first line below
//#define DEBUG_PHYSICS_CPP 0
#ifdef DEBUG_PHYSICS_CPP
#define DEBUGPHYSICS(str)       \
    do{                         \
        std::cerr << str << std::flush;       \
    }while(false)
#else
#define DEBUGPHYSICS(str)       \
    do{                         \
    }while(false)
#endif

using glm::vec2;
using glm::vec3;
using std::vector;
using std::make_pair;

GravitySystem::GravitySystem( const vector<vec2>& _in ){
    for ( vec2 _p : _in )
        particles.push_back( VerletParticle( _p ) );
}

void GravitySystem::sendData(vector<vec3>& points) {
    points.clear();
    for ( VerletParticle& vp : particles )
        points.push_back( vec3( vp.pos( vec2() ), vp.radius) );
}

void GravitySystem::step() {
    clock_t start_time = clock();
    flaggedForCollides.clear();
    // Apply velocity and gravity
    float t = STEP_TIME;
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
        vp.v1 = vp.velocity();
        vp.v1 += t * gForce;
        short f = inBounds( vp );
        if( f != 0 ){
            fixBounds( vp, f );
        }
    }
    executeCollisions(); 
    // Update Particle Data
    for (VerletParticle& vp : particles) {
        std::cout << " Updating Data vp.p.y : " << vp.p.y << " vp.tempPos().y : " << vp.tempPos().y << std::endl;
        vp.p = vp.tempPos();
        vp.v0 = vp.v1;
    }
    // Wait till a frame should be updated
    clock_t end_time = clock();
}


void GravitySystem::executeCollisions() {
    //std::cout << "Executing Collisions." << std::endl;
    for( int l = 0; l < particles.size()-1; ++l ){
        VerletParticle& lhs = particles[l];
        for( int r = l+1; r < particles.size(); ++r){
            VerletParticle& rhs = particles[r];
            if( collides( lhs, rhs ) )
                fixCollision( lhs, rhs );
        }
    }
}

bool GravitySystem::collides( const VerletParticle& lhs, const VerletParticle& rhs ) {
    bool differentParticle = lhs!=rhs;
    bool inContact = glm::distance( lhs.tempPos(), rhs.tempPos() ) < lhs.radius + rhs.radius + 2.0f * float(FLOAT_EPSILON);
    bool movingTowards = glm::distance( lhs.p + (1.0f/200.0f) * lhs.v1, rhs.p + (1.0f/20.0f) * rhs.v1 ) < glm::distance( lhs.p, rhs.p );
    return differentParticle && inContact && movingTowards;
}

void GravitySystem::fixCollision(VerletParticle& lhs, VerletParticle& rhs) {
    //std::cout << "Fixing a collision\n" << std::flush;
    //std::cout << "  in lhs.v1.x : " << lhs.v1.x << " rhs.v1.x : " << rhs.v1.x << std::endl;
    vec3 tempL = lhs.v1;
    vec3 tempR = rhs.v1;
    lhs.v1 = lhs.elasticity * rhs.v1;
    rhs.v1 = rhs.elasticity * tempL;
    //std::cout << " out lhs.v1.x : " << lhs.v1.x << " rhs.v1.x : " << rhs.v1.x << std::endl;
    
    //std::cout << " current Pos lhs : " << lhs.p.x << " rhs : " << rhs.p.x << std::endl;
    vec3 between = rhs.p - lhs.p;
    float length = glm::length( between );
    float shift = length - lhs.radius - rhs.radius;
    shift /= 2.0;
    lhs.p += shift * glm::normalize(tempL);
    rhs.p += shift * glm::normalize(tempR);
    //std::cout << " contact Pos lhs : " << lhs.p.x << " rhs : " << rhs.p.x << std::endl;
    lhs.p -= lhs.v1;
    rhs.p -= rhs.v1;
    //std::cout << " spoofed Pos lhs : " << lhs.p.x << " rhs : " << rhs.p.x << std::endl;
}

// 0 - No Collision
// 1 - Hits West Wall
// 2 - Hits South Wall
// 3 - Hits East Wall
// 4 - Hits North Wall
short GravitySystem::inBounds(const VerletParticle& _p) {
    VerletParticle west = VerletParticle( -_p.radius, _p.tempPos().y );
    VerletParticle south = VerletParticle( _p.tempPos().x, -_p.radius ); 
    VerletParticle east = VerletParticle( width + _p.radius, _p.tempPos().y );
    VerletParticle north = VerletParticle( _p.tempPos().x, height + _p.radius ); 
    if( collides( _p, west ) )
        return 1;
    if( collides( _p, south ) )
        return 2;
    if( collides( _p, east ) )
        return 3;
    if( collides( _p, north ) )
        return 4;
    return 0;
}

void GravitySystem::fixBounds( VerletParticle& _p, const short& flag ) {
    DEBUGPHYSICS("Correcting Bounds.\n");
    if( flag == 1 ){
        _p.v1 = _p.elasticity * _p.v1 * vec3( -1.0, 1.0, 1.0 );
        _p.p = vec3( _p.radius, _p.p.y, _p.p.z) - _p.v1;
	}
    if( flag == 2 ){
        _p.v1 = _p.elasticity * _p.v1 * vec3( 1.0, -1.0, 1.0 );
        _p.p = vec3( _p.p.x, _p.radius, _p.p.z) - _p.v1;
    }
    if( flag == 3 ){
        _p.v1 = _p.elasticity * _p.v1 * vec3( -1.0, 1.0, 1.0 );
        _p.p = vec3( width - _p.radius, _p.p.y, _p.p.z) - _p.v1;
	}
    if( flag == 4 ){
        _p.v1 = _p.elasticity * _p.v1 * vec3( 1.0, -1.0, 1.0 );
        _p.p = vec3( _p.p.x, height - _p.radius, _p.p.z) - _p.v1;
	}
}

#endif
