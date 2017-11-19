#ifndef SCENE_H
#define SCENE_H
#include "ParticleSystem.h"     // ParticleSystem
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <glm/gtc/vec1.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec1;


class Scene{
        ParticleSystem* const rootSystem;
        vector<vec3> points;
    public:
        Scene(const ParticleSystem* _root):rootSystem(const_cast<ParticleSystem* const>(_root)),points(){}
        bool retrieveData(){return rootSystem->sendData(points);}
        bool updateBuffers(vector<vec4>& pointsNDC, vector<uvec1>& pointsNumbers);
        vec2 toScreen(const vec3& _p);
};
#endif
