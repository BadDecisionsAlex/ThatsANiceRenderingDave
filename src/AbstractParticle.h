#ifndef ABSTRACT_PARTICLE_H
#define ABSTRACT_PARTICLE_H
#include <glm/glm.hpp> // vec2, vec3

using glm::vec2;
using glm::vec3;

struct AbstractParticle{
    vec3 p;

    AbstractParticle() {}

    AbstractParticle(float _x, float _y) : p(vec3(_x, _y, 1.0)) {}
    
    virtual ~AbstractParticle() = default;
    virtual vec3 pos() const { return p; }
    virtual vec2 pos(vec2) const { return vec2(p); }
    virtual float x() const { return p.x; }
    virtual float y() const { return p.y; }
    virtual inline bool operator==(const AbstractParticle& rhs)  const { return p==rhs.p; }
};

struct VerletParticle : AbstractParticle{
    
    const float DEFAULT_RADIUS = 3.0;
    const float DEFAULT_ELASTICITY = 0.25;
    int id;

    // Instance Variables
    vec3 v0,v1;
    float radius = DEFAULT_RADIUS;
    float elasticity = DEFAULT_ELASTICITY;
    bool out = false;
    // Constructors
    VerletParticle() : AbstractParticle() {}
    VerletParticle(float _x, float _y) : AbstractParticle(_x,_y), v0( vec3( 0.0, 0.0, 0.0 ) ), v1( v0 ){}
    VerletParticle(vec2 _p) : VerletParticle( _p.x, _p.y ){}
    ~VerletParticle()=default;
    // Calculations/Getters
    vec3 velocity() const {return v0;}
    vec3 velocity(vec3) const {return velocity();}
    vec3 acceleration() const {return v1 - v0;}
    vec3 previousPos() const {return p - v0;}
    vec3 tempPos() const {return p + v1;}
    // Comparison
    inline bool operator==(const VerletParticle& rhs) const {return AbstractParticle::operator==(rhs)&&(v0==rhs.v0)&&(v1==rhs.v1);}
    inline bool operator!=(const VerletParticle& rhs) const {return !(operator==(rhs));}
};
#endif
