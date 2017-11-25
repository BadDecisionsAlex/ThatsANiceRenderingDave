# ifndef SCENE_CPP
# define SCENE_CPP
# include "Scene.h"
using glm::vec2;
using glm::vec3;
using glm::vec4;

void Scene::updateBuffers(vector<vec4>& pointsNDC, vector<uvec1>& pointsNumbers){
	pointsNDC.clear();
	pointsNumbers.clear();
	int pointCounter = 0;
	for(vec3 _p : points) {
		pointsNDC.push_back(vec4(toScreen(_p), 0.0, 1.0));
		pointsNumbers.push_back(uvec1(pointCounter));
		pointCounter += 1;
	}
}

vec2 Scene::toScreen(const vec3& _p){
	float ndcX = ( ( 2.0f * _p.x ) / float(rootSystem->width) ) - 1.0f;
	float ndcY = ( ( 2.0f * _p.y ) / float(rootSystem->height) ) - 1.0f;
	return vec2( ndcX, ndcY );
}

# endif
