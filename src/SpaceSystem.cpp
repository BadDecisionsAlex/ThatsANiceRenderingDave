//
//  SpaceSystem.cpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#include <stdio.h>
#include "SpaceSystem.h"
#include "Shaders.h"

void SpaceSystem::setup() {
    //Add particles
    for (int c = 0; c < 100000; ++c) {
        int x = rand() % 1000;
        int y = rand() % 1000;
        float z = -(rand() % 10000 / 10000.0);
        
        float dx = (rand() % 1000) / 500.0 - 1;
        float dy = (rand() % 1000) / 500.0 - 1;
        int life = rand() % 10000;
        
        MassParticle particle = MassParticle(vec3(x, y, z));
        particle.velocity = vec3(0, 0, 0);
        particle.life = life;
        
        particles.push_back(particle);
    }
    
    getPointsForScreen(points, indices);
}

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
    vec4 point = toScreen(center);
    points.push_back(point);
    indices.push_back(uvec1(count));
}

vec4 SpaceSystem::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.x) / float(width * 2)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height * 2)) - 1.0f;
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

    stepCount += 1;
    if (stepCount >= 360) {
        stepCount = 0;
    }
    
    float r = (stepCount / 360.0) * (2 * 3.14159);
    
    vec3 c1 = vec3(cosf(r), sinf(r), 0);
    vec3 c2 = vec3(-sinf(r), cosf(r), 0);
    vec3 c3 = vec3(0, 0, 1);
    glm::mat3 m(c1, c2, c3);
    
    vec3 direction(200, 0, 0);
    center = vec3(1000, 1000, 0) + (m * direction);
    
    for (MassParticle& p : particles) {
        vec3 direction = glm::normalize(center - p.p);
        direction *= ((16.0 / 600.0) * 9.807);
        p.velocity += direction;
        
        p.p += p.velocity;
    }
}


//MARK: - Draw

SpaceSystem::SpaceSystem() : particle_pass(-1, particle_pass_input, { particle_vertex_shader, particle_geometry_shader, particle_fragment_shader }, { /* uniforms */ }, { "fragment_color" }) {
    getPointsForScreen(points, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
}

void SpaceSystem::prepareDraw() {
    particle_pass_input.assign_index(indices.data(), indices.size(), 1);
    particle_pass = RenderPass(-1,
                               particle_pass_input,
                               {
                                   particle_vertex_shader,
                                   particle_geometry_shader,
                                   particle_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
                               );
}

void SpaceSystem::draw() {
    getPointsForScreen(points, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}


