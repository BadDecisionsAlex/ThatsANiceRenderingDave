# ifndef SCENE_CPP
# define SCENE_CPP
# include "Scene.h"
using glm::vec2;
using glm::vec3;
using glm::vec4;

bool Scene::updateBuffers(vector<vec4>& pointsNDC){
	pointsNDC.clear();
	for(vec3 _p : points)
		pointsNDC.push_back(vec4(toScreen(_p),0.0,1.0));
    return true;
}

vec2 Scene::toScreen(const vec3& _p){
	float ndcX = ( 2.0f * _p.x ) / rootSystem->width - 1.0;
	float ndcY = ( 2.0f * _p.y ) / rootSystem->height - 1.0;
	return vec2( ndcX, ndcY );
}

# endif
