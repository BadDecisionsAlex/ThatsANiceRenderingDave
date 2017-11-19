#ifndef SCENE_H
#define SCENE_H
#include "ParticleSystem.h"     // ParticleSystem
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
using glm::vec2;
using glm::vec3;

class Scene{
        ParticleSystem* const rootSystem;
        vector<vec3> points;
    public:
        Scene(const ParticleSystem* _root):rootSystem(const_cast<ParticleSystem* const>(_root)),points(){}
        bool retrieveData(){return rootSystem->sendData(points);}
        bool updateBuffers();
};
#endif
