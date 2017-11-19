#ifndef ABSTRACT_PARTICLE_H
#define ABSTRACT_PARTICLE_H
#include <glm/glm.hpp> // vec2, vec3
using glm::vec2;
using glm::vec3;
struct AbstractParticle{
    vec3 p;
    AbstractParticle(float _x, float _y):p(vec3(_x,_y,1.0)){}
    AbstractParticle(vec2 _p):p(vec3(_p,1.0)){}
    AbstractParticle(vec3 _p):p(_p){}
    virtual ~AbstractParticle()=default;
    virtual vec2 pos(){return vec2(p);}
    virtual vec2 pos(vec2){return pos();}
    virtual vec3 pos(vec3){return p;}
    virtual float x(){return p.x;}
    virtual float y(){return p.y;}
};

struct VerletParticle : AbstractParticle{
    constexpr static float DEFAULT_RADIUS = 0.5;
    vec3 p1;
    float radius;
    VerletParticle(float _x, float _y):AbstractParticle(_x,_y), p1(vec3(_x,_y,1.0)), radius(DEFAULT_RADIUS){}
    VerletParticle(float _x, float _y, float _r):AbstractParticle(_x,_y), p1(vec3(_x,_y,1.0)), radius(DEFAULT_RADIUS){}
    VerletParticle(float _x, float _y, float _x1, float _y1):AbstractParticle(_x,_y), p1(vec3(_x1,_y1,1.0)), radius(DEFAULT_RADIUS){}
    VerletParticle(float _x, float _y, float _x1, float _y1, float _r):AbstractParticle(_x,_y), p1(vec3(_x,_y,1.0)), radius(_r){}
    VerletParticle(vec2 _p):AbstractParticle(_p), p1(vec3(_p,1.0)), radius(DEFAULT_RADIUS){}
    VerletParticle(vec2 _p, float _r):AbstractParticle(_p), p1(vec3(_p,1.0)), radius(_r){}
    VerletParticle(vec2 _p, vec2 _p1):AbstractParticle(_p), p1(vec3(_p1,1.0)), radius(DEFAULT_RADIUS){}
    VerletParticle(vec2 _p, vec2 _p1, float _r):AbstractParticle(_p), p1(vec3(_p1,1.0)), radius(_r){}
    VerletParticle(vec3 _p):AbstractParticle(_p), p1(_p), radius(DEFAULT_RADIUS){}
    VerletParticle(vec3 _p, float _r):AbstractParticle(_p), p1(_p), radius(_r){}
    VerletParticle(vec3 _p, vec3 _p1):AbstractParticle(_p), p1(_p1), radius(DEFAULT_RADIUS){}
    VerletParticle(vec3 _p, vec3 _p1, float _r):AbstractParticle(_p), p1(_p1), radius(_r){}
    ~VerletParticle()=default;
    vec3 velocity(){return p-p1;}
    vec3 velocity(vec3){return velocity();}
    vec2 velocity(vec2){return vec2(velocity());}
    vec2 previousPos(){return vec2(p1);}
    vec2 previousPos(vec2){return previousPos();}
    vec3 previousPos(vec3){return p1;}
    float prevx(){return p1.x;}
    float prevy(){return p1.y;}
    float deltax(){return velocity().x;}
    float deltay(){return velocity().y;}
};
#endif
