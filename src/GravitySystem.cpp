#ifndef PARTICLE_SYSTEM_CPP
#define PARTICLE_SYSTEM_CPP
#include "GravitySystem.h"
#include "Shaders.h"
#include "SpaceSystem.h"
#include <iostream>                 // std::cout
#include <glm/gtx/string_cast.hpp>  // glm::to_string

#define STEP_TIME 1.0f / 600.0f
#define NUMBER_OF_PARTICLES 2500.0

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

using std::make_pair;



// Gravity System

void GravitySystem::step() {
//    if(step_count % 60 == 0){
//        addParticle();
//    }
    clock_t start_time = clock();
    flaggedForCollides.clear();
    // Apply velocity and gravity
    float t = STEP_TIME;
    vec3 grav = t * gForce;
    for (int a = 0; a < particles.size(); ++a) {
        VerletParticle& vp = particles[a];
        vp.out = false;
        vp.v1 = vp.velocity();
        vp.v1 += grav;
        if( glm::length(vp.v1) >= 10.0 )
            vp.v1 /= 2.0;
        fixBounds( vp );
    }
    executeCollisions();
    // Update Particle Data
    for (VerletParticle& vp : particles) {
//        std::cout << " Updating Data oldVelocity.p.y : " << oldVelocity.p.y << " oldVelocity.tempPos().y : " << oldVelocity.tempPos().y << std::endl;
        vp.p = vp.tempPos();
        vp.v0 = vp.v1; //drag
    }
    // Wait till a frame should be updated
    clock_t end_time = clock();
    ++step_count;
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
    float t = float(STEP_TIME);
    t*=t;
    bool differentParticle = lhs!=rhs;
    bool inContact = glm::distance( lhs.tempPos(), rhs.tempPos() ) < lhs.radius + rhs.radius + 2.0f * float(FLOAT_EPSILON);
    bool movingTowards = glm::distance( lhs.p + t * lhs.v1, rhs.p + t * rhs.v1 ) < glm::distance( lhs.p, rhs.p );
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
    //VerletParticle west = VerletParticle( -_p.radius, _p.tempPos().y );
    //VerletParticle south = VerletParticle( _p.tempPos().x, -_p.radius ); 
    //VerletParticle east = VerletParticle( width + _p.radius, _p.tempPos().y );
    //VerletParticle north = VerletParticle( _p.tempPos().x, height + _p.radius ); 
    if( /*collides( _p, west ) ) || */ _p.tempPos().x <= _p.radius && _p.p.x > _p.radius )
        return 1;
    if( /*collides( _p, south ) ) || */ _p.tempPos().y <= _p.radius && _p.p.y > _p.radius )
        return 2;
    if( /*collides( _p, east ) ) || */ _p.tempPos().x >= width- _p.radius && _p.p.x < width - _p.radius )
        return 3;
    if( /*collides( _p, north ) ) || */ _p.tempPos().y >= height - _p.radius && _p.p.y < height - _p.radius )
        return 4;
    return 0;
}

void GravitySystem::fixBounds( VerletParticle& _p, const short& flag ) {
    DEBUGPHYSICS("Correcting Bounds.\n");
    if( flag == 1 ){
        _p.p = _p.tempPos();
        //_p.v1 = /*_p.elasticity **/ _p.v1 * vec3( -1.0, 1.0, 1.0 );
        //_p.out = true;
        _p.v1.x *= -1.0;
	}
    if( flag == 2 ){
        _p.p = _p.tempPos();
        //_p.v1 = /*_p.elasticity **/ _p.v1 * vec3( 1.0, -1.0, 1.0 );
        //_p.out = true;
        _p.v1.y *= -1.0;
    }
    if( flag == 3 ){
        _p.p = _p.tempPos();
        //_p.v1 = /*_p.elasticity **/ _p.v1 * vec3( -1.0, 1.0, 1.0 );
        //_p.out = true;
        _p.v1.x *= -1.0;
	}
    if( flag == 4 ){
        _p.p = _p.tempPos();
        //_p.v1 = /*_p.elasticity **/ _p.v1 * vec3( 1.0, -1.0, 1.0 );
        //_p.out = true;
        _p.v1.y *= -1.0;
	}
    //executeCollisions();
}

void GravitySystem::fixBounds( VerletParticle& _p ){
    short flag = inBounds(_p);
    if(flag!=0)
        fixBounds( _p, flag );
}

//Draw

GravitySystem::GravitySystem() : particle_pass(-1, particle_pass_input, { particle_vertex_shader, collision_geometry_shader, particle_fragment_shader}, {/* uniforms */}, { "fragment_color"}){
    getPointsForScreen(points, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
}

void GravitySystem::getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    int count = 0;
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
    particle_pass_input.assign_index(indices.data(), indices.size(), 1);
    particle_pass = RenderPass(-1,
                               particle_pass_input,
                               {
                                       particle_vertex_shader,
                                       collision_geometry_shader,
                                       particle_fragment_shader
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

void GravitySystem::addParticle(){
    int x = rand() % 1000;
    int y = rand() % 1000;

    float dx = (rand() % 1000) / 500.0 - 1;
    float dy = (rand() % 1000) / 500.0 - 1;
    VerletParticle particle = VerletParticle(x, y);
    particles.push_back(particle);
}

void GravitySystem::setup() {
    //Add particles
    for (int c = 0; c < NUMBER_OF_PARTICLES; ++c) {
        int x = rand() % 1000;
        int y = rand() % 1000;

        float dx = (rand() % 1000) / 500.0 - 1;
        float dy = (rand() % 1000) / 500.0 - 1;

        VerletParticle particle = VerletParticle(x, y);
        int q = 100;
        int p = 1;
        particle.v0 = vec3(q,p,0);
        particles.push_back(particle);
    }
    grid = ParticleGrid(10, width, height);

    getPointsForScreen(points, indices);
}
#endif
