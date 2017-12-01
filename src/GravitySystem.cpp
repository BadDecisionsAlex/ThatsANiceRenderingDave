#ifndef PARTICLE_SYSTEM_CPP
#define PARTICLE_SYSTEM_CPP
#include "GravitySystem.h"
#include <iostream>                 // std::cout
#include <glm/gtx/string_cast.hpp>  // glm::to_string

#define STEP_TIME 1.0f / 600.0f

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

using glm::uvec1;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::vector;
using std::make_pair;

const char* particle_vertex_shader1 =
#include "shaders/particle.vert"
;

const char* particle_geometry_shader1 =
#include "shaders/particle.geom"
;

const char* particle_fragment_shader1 =
#include "shaders/particle.frag"
;

//Draw

GravitySystem::GravitySystem(const vector<VerletParticle>& _in) : particles(_in), grid(10.0f, width, height), particle_pass(-1, particle_pass_input, { particle_vertex_shader1, particle_geometry_shader1, particle_fragment_shader1 }, { /* uniforms */ }, { "fragment_color" }) {
}

void GravitySystem::getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    int count = 1;
    for (VerletParticle& vp : particles) {
        vec4 point = toScreen(vp.p);
        points.push_back(point);
        indices.push_back(uvec1(count));
        ++count;
    }
}

vec4 GravitySystem::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.x) / float(width)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0, 1.0);
}

void GravitySystem::prepareDraw() {
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
    particle_pass_input.assign_index(indices.data(), indices.size(), 1);
    particle_pass = RenderPass(-1,
                             particle_pass_input,
                             {
                                 particle_vertex_shader1,
                                 particle_geometry_shader1,
                                 particle_fragment_shader1
                             },
                             { /* uniforms */ },
                             { "fragment_color" }
                             );
}

void GravitySystem::draw() {
    getPointsForScreen(points, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}


// Gravity System

void GravitySystem::step() {
    clock_t start_time = clock();
    flaggedForCollides.clear();
    // Apply velocity and gravity
    float t = STEP_TIME;
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
        vp.out = false;
        vp.v1 = vp.velocity();
        vp.v1 += t * gForce;
        fixBounds( vp );
    }
    executeCollisions();
    // Update Particle Data
    for (VerletParticle& vp : particles) {
//        std::cout << " Updating Data vp.p.y : " << vp.p.y << " vp.tempPos().y : " << vp.tempPos().y << std::endl;
        vp.p = vp.tempPos();
        vp.v0 = vp.v1;
    }
    // Wait till a frame should be updated
    clock_t end_time = clock();
}


void GravitySystem::executeCollisions() {
    //std::cout << "Executing Collisions." << std::endl;
    //for( int l = 0; l < particles.size() - 1; ++l ){
    //    VerletParticle& lhs = particles[l];
    //    for( int r = l + 1; r < particles.size(); ++r){
    //        VerletParticle& rhs = particles[r];
    //        if( collides( lhs, rhs ) )
    //            fixCollision( lhs, rhs );
    //    }
    //}
    grid.update(particles);
    for (int i = 0; i < particles.size(); ++i) {
        VerletParticle& lhs = particles[i];
        vector<VerletParticle> canidates = grid.collides(lhs);
        //std::cout << "PArticles : " << particles.size() << std::endl;
        //std::cout << "Candidates size : " << canidates.size() << std::endl;
        for (int j = 0; j < canidates.size(); ++j) {
            VerletParticle& rhs = canidates[j];
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
    vec3 forceL = lhs.elasticity * lhs.v1;
    vec3 forceR = rhs.elasticity * rhs.v1;
    //if(lhs.out && !rhs.out){
    //    forceR = vec3( 0.0, 0.0, 0.0 );
    //    forceL -= forceR;
    //}else if(rhs.out && !lhs.out){
    //    forceL = vec3( 0.0, 0.0, 0.0 );
    //    forceR -= forceL;
    //}else if(lhs.out && rhs.out){
    //    forceL = vec3( 0.0, - 1.02f * lhs.radius, 0.0 );
    //    forceR = vec3( 0.0, 0.0, 0.0 );
    //}
    lhs.v1 += forceR;
    rhs.v1 += forceL;
    fixBounds(lhs);
    fixBounds(rhs);
}

// 0 - No Collision
// 1 - Hits West Wall
// 2 - Hits South Wall
// 3 - Hits East Wall
// 4 - Hits North Wall
//
// 11 - Way out West
// 12 - Way out South
// 13 - Way out East
// 14 - Way out North
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
        //_p.p = _p.tempPos();
        _p.v1 = _p.elasticity * _p.v1 * vec3( -1.0, 1.0, 1.0 );
        _p.out = true;
	}
    if( flag == 2 ){
        //_p.p = _p.tempPos();
        _p.v1 = _p.elasticity * _p.v1 * vec3( 1.0, -1.0, 1.0 );
        _p.out = true;
    }
    if( flag == 3 ){
        //_p.p = _p.tempPos();
        _p.v1 = _p.elasticity * _p.v1 * vec3( -1.0, 1.0, 1.0 );
        _p.out = true;
	}
    if( flag == 4 ){
        //_p.p = _p.tempPos();
        _p.v1 = _p.elasticity * _p.v1 * vec3( 1.0, -1.0, 1.0 );
        _p.out = true;
	}
    //executeCollisions();
}

void GravitySystem::fixBounds( VerletParticle& _p ){
    short flag = inBounds(_p);
    if(flag!=0)
        fixBounds( _p, flag );
}

#endif
