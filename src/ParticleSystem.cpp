#ifndef PARTICLE_SYSTEM_CPP
#define PARTICLE_SYSTEM_CPP
#include "ParticleSystem.h"
#include <iostream>                 // std::cout
#include <glm/gtx/string_cast.hpp>  // glm::to_string

#define STEP_TIME 1.0f / 60.0f

// To disable debug messages for this file comment out the first line below
//#define DEBUG_PHYSICS_CPP 0
#ifdef DEBUG_PHYSICS_CPP
#define DEBUGPHYSICS(str)       \
    do{                         \
        std::cerr << str;       \
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

GravitySystem::GravitySystem(vec3  _g, const vector<vec2>& _in) : gForce(_g) {
    for (vec2 _p : _in)
        particles.push_back(VerletParticle(_p));
}

bool GravitySystem::sendData(vector<vec3>& points) {
    points.clear();
    for (VerletParticle& vp : particles){
        points.push_back(vec3(vp.pos(),vp.radius));
    }
    return true;
}

bool GravitySystem::step() {
    //DEBUGPHYSICS("Gravity System is initiating a step.\n" );
    clock_t start_time = clock();
    float t = 1.0/60.0; // 60 FPS
    // Apply velocity and gravity
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
		vp.p0 += vp.velocity();
        vp.p0 += t * gForce;
        fixBounds(vp);
        DEBUGPHYSICS("p : " << vp.p.y << "\t\tvelocity : " << vp.velocity().y << "\t\tacceleration : " << vp.acceleration().y << '\n');
    }
    // todo : change this function to calcCollisions which is function that other particle systems can overide
    correctCollides();
    // Update Particle Data
    for (VerletParticle& vp : particles) {
        vp.p1 = vp.p;
        vp.p = vp.p0;
    }
    DEBUGPHYSICS('\n');
    // Wait till a frame should be updated
    clock_t end_time = clock();
    return true;
}

// todo : change this function name to calcCollisions which is a better name
bool GravitySystem::correctCollides() {
    //DEBUGPHYSICS("Correcting Collisions.\n");
	bool done = true;
	for(int a=0; a < particles.size()-1; ++a){
		VerletParticle& pA = particles[a];
		for(int b=a+1;b<particles.size();++b){
			VerletParticle& pB = particles[b];
			if( collides( pA, pB ) ){
				done = false;
                // We must determine the value t where collision occurred.
                // accuracy is the number of subdivisions to check between 0 < t < 1
                float t = 0;
                float accuracy = 1.0/10.0;
                vec2 tOfA = pA.pos();
                vec2 tOfB = pB.pos();
                vec2 deltaA = pA.tempPos() - tOfA;
                vec2 deltaB = pB.tempPos() - tOfB;
                while(glm::distance( tOfA, tOfB ) > pA.radius + pB.radius + 2 * FLOAT_EPSILON ){
                    tOfA += accuracy * deltaA;
                    tOfB += accuracy * deltaB;
                    t += accuracy;
                    if ( t > 1.000001){
                        std::cerr << "Particle Collision detection error." << std::endl;
                        break;
                    }
                }
                DEBUGPHYSICS("Collision occurs at t : " << t << '\n');
                DEBUGPHYSICS("tOfA : " << tOfA.y << " tOfB : " << tOfB.y << '\n');
                DEBUGPHYSICS("VelocityA : " << pA.velocity().y << " VelocityB : " << pB.velocity().y << '\n');
                // Velocity right after collision
                vec2 velocityAt = pA.velocity(vec2()) + t * pA.acceleration(vec2()); // V1
                vec2 velocityBt = pB.velocity(vec2()) + t * pB.acceleration(vec2()); // V2
                DEBUGPHYSICS("VelocityAofT : " << velocityAt.y << " VelocityBofT : " << velocityBt.y << '\n');
                // Calculate force applied to each particle.
                vec2 newVelocityA = velocityAt - velocityBt;
                vec2 newVelocityB = velocityBt - velocityAt;
                pA.p = vec3( tOfA - t * newVelocityA, 1.0 );
                pB.p = vec3( tOfB - t * newVelocityB, 1.0 );
                DEBUGPHYSICS("pA.p0 was : " << pA.p0.y << " pB.p0 was : " << pB.p0.y << '\n');
                pA.p0 = vec3( tOfA + ( (1.0f-t) * newVelocityA ), 1.0 );
                pB.p0 = vec3( tOfB + ( (1.0f-t) * newVelocityB ), 1.0 );
                DEBUGPHYSICS("pA.p0 is : " << pA.p0.y << " pB.p0 is : " << pB.p0.y << "\n\n");
			}
		}
	}
	//if(!done)
	//	correctCollides();
	return true;
}

// todo : might have to be renamed of reimplemented to properly conform to a grid system
bool GravitySystem::collides(const VerletParticle& lhs, const VerletParticle& rhs) {
    return (lhs!=rhs)&&( glm::distance( lhs.tempPos(), rhs.tempPos() ) <lhs.radius+rhs.radius+2*FLOAT_EPSILON);
}

void GravitySystem::fixBounds(VerletParticle& _p) {
    DEBUGPHYSICS("Correcting Bounds.\n");
    if(_p.p0.x-_p.radius<FLOAT_EPSILON && _p.velocity().x < FLOAT_EPSILON){
      	_p.p.x = _p.p0.x;
		_p.p0.x = _p.radius;
	}
    if(_p.p0.y-_p.radius<FLOAT_EPSILON && _p.velocity().y < FLOAT_EPSILON){
		_p.p.y = _p.p0.y;
      	_p.p0.y = _p.radius;
    }
    if(_p.p0.x+_p.radius>width-FLOAT_EPSILON){
		_p.p.x = _p.p0.x;
      	_p.p0.x = width - _p.radius;
	}
    if(_p.p0.y+_p.radius>height-FLOAT_EPSILON){
		_p.p.y = _p.p0.y;
      	_p.p0.y = height - _p.radius;
	}
}

const vec3 GravitySystem::DEFAULT_GRAVITY = vec3( 0.0, -9.807, 0.0);

bool GriddedGravitySystem::step() {
    //DEBUGPHYSICS("Gravity System is initiating a step.\n" );
    clock_t start_time = clock();
    float t = 1.0/60.0; // 60 FPS
    // Apply velocity and gravity
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
		vp.p0 += vp.velocity();
        vp.p0 += t * gForce;
        fixBounds(vp);
        DEBUGPHYSICS("p : " << vp.p.y << "\t\tvelocity : " << vp.velocity().y << "\t\tacceleration : " << vp.acceleration().y << '\n');
    }
    // todo : change this function to calcCollisions which is function that other particle systems can overide
    GriddedGravitySystem::correctCollides();
    // Update Particle Data
    for (VerletParticle& vp : particles) {
        vp.p1 = vp.p;
        vp.p = vp.p0;
    }
    DEBUGPHYSICS('\n');
    // Wait till a frame should be updated
    clock_t end_time = clock();
    return true;
}


bool GriddedGravitySystem::correctCollides(){
    //DEBUGPHYSICS("Correcting Collisions.\n");
    bool done = true;
    grid.update(particles);
    for (int i = 0; i < particles.size(); ++i) {
        VerletParticle& pA = particles[i];
        vector<VerletParticle> canidates = grid.collides(pA);
        for (int j = 0; j < canidates.size(); ++j) {
           VerletParticle& pB = particles[j];
            if( collides( pA, pB ) ){
                done = false;
                // We must determine the value t where collision occurred.
                // accuracy is the number of subdivisions to check between 0 < t < 1
                float t = 0;
                float accuracy = 1.0/10.0;
                vec2 tOfA = pA.pos();
                vec2 tOfB = pB.pos();
                vec2 deltaA = pA.tempPos() - tOfA;
                vec2 deltaB = pB.tempPos() - tOfB;
                while(glm::distance( tOfA, tOfB ) > pA.radius + pB.radius + 2 * FLOAT_EPSILON ){
                    tOfA += accuracy * deltaA;
                    tOfB += accuracy * deltaB;
                    t += accuracy;
                    if ( t > 1.000001){
                        std::cerr << "Particle Collision detection error." << std::endl;
                        break;
                    }
                }
                DEBUGPHYSICS("Collision occurs at t : " << t << '\n');
                DEBUGPHYSICS("tOfA : " << tOfA.y << " tOfB : " << tOfB.y << '\n');
                DEBUGPHYSICS("VelocityA : " << pA.velocity().y << " VelocityB : " << pB.velocity().y << '\n');
                // Velocity right after collision
                vec2 velocityAt = pA.velocity(vec2()) + t * pA.acceleration(vec2()); // V1
                vec2 velocityBt = pB.velocity(vec2()) + t * pB.acceleration(vec2()); // V2
                DEBUGPHYSICS("VelocityAofT : " << velocityAt.y << " VelocityBofT : " << velocityBt.y << '\n');
                // Calculate force applied to each particle.
                vec2 newVelocityA = velocityAt - velocityBt;
                vec2 newVelocityB = velocityBt - velocityAt;
                pA.p = vec3( tOfA - t * newVelocityA, 1.0 );
                pB.p = vec3( tOfB - t * newVelocityB, 1.0 );
                DEBUGPHYSICS("pA.p0 was : " << pA.p0.y << " pB.p0 was : " << pB.p0.y << '\n');
                pA.p0 = vec3( tOfA + ( (1.0f-t) * newVelocityA ), 1.0 );
                pB.p0 = vec3( tOfB + ( (1.0f-t) * newVelocityB ), 1.0 );
                DEBUGPHYSICS("pA.p0 is : " << pA.p0.y << " pB.p0 is : " << pB.p0.y << "\n\n");
            }
        }
    }
    return true;
}


#endif
