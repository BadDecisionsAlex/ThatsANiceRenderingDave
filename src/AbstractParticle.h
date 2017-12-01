#ifndef ABSTRACT_PARTICLE_H
#define ABSTRACT_PARTICLE_H
#include <glm/glm.hpp> // vec2, vec3

using glm::vec2;
using glm::vec3;

struct AbstractParticle {
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

#endif
