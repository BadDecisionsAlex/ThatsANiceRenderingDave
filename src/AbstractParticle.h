#ifndef ABSTRACT_PARTICLE_H
#define ABSTRACT_PARTICLE_H

#include <glm/glm.hpp> // vec2, vec3

using glm::vec2;
using glm::vec3;

struct AbstractParticle {
    vec3 p;

    AbstractParticle(float _x, float _y) : p(vec3(_x, _y, 1.0)) {}

    AbstractParticle(vec2 _p) : p(vec3(_p, 1.0)) {}

    AbstractParticle(vec3 _p) : p(_p) {}

    virtual ~AbstractParticle() = default;

    virtual vec2 pos() const { return vec2(p); }

    virtual vec2 pos(vec2) const { return pos(); }

    virtual vec3 pos(vec3) const { return p; }

    virtual float x() const { return p.x; }

    virtual float y() const { return p.y; }

    virtual inline bool operator==(const AbstractParticle &rhs) const { return p == rhs.p; }
};

struct VerletParticle : AbstractParticle {
    int id;
    constexpr static float DEFAULT_RADIUS = 5.0;
    // Instance Variables
    vec3 p0, p1;
    float radius;

    // Constructors
    VerletParticle(float _x, float _y, int id) : AbstractParticle(_x, _y), p0(p), p1(p), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(float _x, float _y, float _r, int id) : AbstractParticle(_x, _y), p0(p), p1(p), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(float _x, float _y, float _x1, float _y1, int id) : AbstractParticle(_x, _y), p0(p), p1(vec3(_x1, _y1, 1.0)),
                                                               radius(DEFAULT_RADIUS) {}

    VerletParticle(float _x, float _y, float _x1, float _y1, float _r, int id) : AbstractParticle(_x, _y), p0(p),
                                                                         p1(vec3(_x, _y, 1.0)), radius(_r), id(id) {}

    VerletParticle(vec2 _p, int id) : AbstractParticle(_p), p0(p), p1(p), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(vec2 _p, float _r, int id) : AbstractParticle(_p), p0(p), p1(p), radius(_r), id(id) {}

    VerletParticle(vec2 _p, vec2 _p1, int id) : AbstractParticle(_p), p0(p), p1(vec3(_p1, 1.0)), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(vec2 _p, vec2 _p1, float _r, int id) : AbstractParticle(_p), p0(p), p1(vec3(_p1, 1.0)), radius(_r), id(id) {}

    VerletParticle(vec3 _p, int id) : AbstractParticle(_p), p0(p), p1(p), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(vec3 _p, float _r, int id) : AbstractParticle(_p), p0(p), p1(p), radius(_r), id(id) {}

    VerletParticle(vec3 _p, vec3 _p1, int id) : AbstractParticle(_p), p0(p), p1(_p1), radius(DEFAULT_RADIUS), id(id) {}

    VerletParticle(vec3 _p, vec3 _p1, float _r, int id) : AbstractParticle(_p), p0(p), p1(_p1), radius(_r), id(id) {}

    ~VerletParticle() = default;

    // Calculations/Getters
    vec3 velocity() const { return p - p1; }

    vec3 velocity(vec3) const { return velocity(); }

    vec2 velocity(vec2) const { return vec2(velocity()); }

    vec3 acceleration() const { return (p0 - p1) - velocity(); }

    vec3 acceleration(vec3) const { return acceleration(); }

    vec2 acceleration(vec2) const { return vec2(acceleration()); }

    vec2 previousPos() const { return vec2(p1); }

    vec2 previousPos(vec2) const { return previousPos(); }

    vec3 previousPos(vec3) const { return p1; }

    float prevx() const { return p1.x; }

    float prevy() const { return p1.y; }

    float deltax() const { return velocity().x; }

    float deltay() const { return velocity().y; }

    vec2 tempPos() const { return vec2(p0); }

    vec2 tempPos(vec2) const { return tempPos(); }

    vec3 tempPos(vec3) const { return p0; }

    float tempx() const { return p0.x; }

    float tempy() const { return p0.y; }

    int getId() const { return id; }

    // Comparison
    inline bool operator==(const VerletParticle &rhs) const {
        return AbstractParticle::operator==(rhs) && (p0 == rhs.p0) && (p1 == rhs.p1);
    }

    inline bool operator!=(const VerletParticle &rhs) const { return !(operator==(rhs)); }
};

#endif
