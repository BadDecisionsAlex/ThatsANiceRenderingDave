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

    GravitySystem();

    void step();
    void setup();
    
    //drawing
    void prepareDraw();
    void draw();
    void getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices);
    
private:
    ParticleGrid grid;
    int step_count = 0;
    
    //Rendering (Could be made simpler)
    RenderDataInput particle_pass_input;
    RenderPass particle_pass;
    vector<vec4> points;
    vector<uvec1> indices;
    
    
    void executeCollisions();
    bool collides( const VerletParticle& lhs, const VerletParticle& rhs );
    void fixCollision( VerletParticle& lsh, VerletParticle& rhs );
    short inBounds( const VerletParticle& _p );
    void fixBounds( VerletParticle& _p, const short& flag );
    void fixBounds( VerletParticle& _p );
    void addParticle();

    glm::vec4 toScreen(const vec3& point);
};


#endif /* GravitySystem_h */
