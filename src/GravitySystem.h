//
//  GravitySystem.h
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef GravitySystem_h
#define GravitySystem_h

#include <glm/gtc/vec1.hpp>

#include "ParticleSystem.h"
#include "ParticleGrid.h"

class GravitySystem : public ParticleSystem {
    const vec3 DEFAULT_GRAVITY = vec3( 0.0, -9.807, 0.0 );
    vector<pair<int,int>> flaggedForCollides;
public:
    vector<VerletParticle> particles;
    vec3 gForce = DEFAULT_GRAVITY;
    
    GravitySystem(const vector<VerletParticle>& _in);
    
    void step();
    
    //drawing
    void prepareDraw();
    void draw();
    
private:
    ParticleGrid grid;
    
    //Rendering (Could be made simpler)
    RenderDataInput particle_pass_input;
    RenderPass particle_pass;
    vector<glm::vec4> points;
    vector<glm::uvec1> indices;
    
    
    void executeCollisions();
    bool collides( const VerletParticle& lhs, const VerletParticle& rhs );
    void fixCollision( VerletParticle& lsh, VerletParticle& rhs );
    short inBounds( const VerletParticle& _p );
    void fixBounds( VerletParticle& _p, const short& flag );
    void fixBounds( VerletParticle& _p );
    
    void getPointsForScreen(vector<glm::vec4>& points, vector<glm::uvec1>& indices);
    glm::vec4 toScreen(const vec3& point);
};


#endif /* GravitySystem_h */
