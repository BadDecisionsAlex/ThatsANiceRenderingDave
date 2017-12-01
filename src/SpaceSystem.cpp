//
//  SpaceSystem.cpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#include <stdio.h>
#include "SpaceSystem.h"

void SpaceSystem::getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    int count = 0;
    for (MassParticle& p : particles) {
        vec4 point = toScreen(p.p);
        points.push_back(point);
        indices.push_back(uvec1(count));
        ++count;
    }
}

vec4 SpaceSystem::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.x) / float(width)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0, 1.0);
}

void SpaceSystem::step() {
    //find center of mass
//    vec3 center(0, 0, 0);
//    int count = 0;
//    for (MassParticle& p : particles) {
//        center += p.p;
//        count += 1;
//    }
//
//    if (count <= 0)
//        return;
//    center /= count;
    
//    vector<int> toRemove;
//    for (int x = 0; x < particles.size(); ++x) {
//        MassParticle& p = particles[x];
//        p.age += 1;
//
//        if (p.age >= p.life) {
//            toRemove.push_back(x);
//        }
//    }
//
//    for (int x = toRemove.size() - 1; x >= 0; --x) {
//        int index = toRemove[x];
//        particles.erase(particles.begin() + index);
//    }

    
    vec3 center(500, 500, 0);
    
    for (MassParticle& p : particles) {
        vec3 direction = glm::normalize(center - p.p);
        direction *= ((2.0 / 600.0) * 9.807);
        p.velocity += direction;
        
        p.p += p.velocity;
    }
}
