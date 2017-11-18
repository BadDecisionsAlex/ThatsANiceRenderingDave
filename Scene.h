#ifndef Scene
#define Scene
#include "AbstractParticle.h"   // AbstractParticle
#include "ParticleSystem.h"     // ParticleSystem
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
using glm::vec2;
using glm::vec3;

class Scene{
        const ParticleSystem* rootSystem;
        vector<vec3> points;
    public:
        Scene(const ParticleSystem* _root):Scene(), rootSystem(_root){}
        bool retrieveData(){rootSystem.sendData(points);}
        bool updateBuffers();
};
#endif
