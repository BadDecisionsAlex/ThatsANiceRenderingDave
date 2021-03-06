//
//  SpaceSystem.cpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#include <stdio.h>
#include "SpaceSystem.h"
#include "Shaders.h"

#define NUMBER_OF_PARTICLES 100000.0
#define MASS 1000

void SpaceSystem::setup() {
    //Add particles
    for (int c = 0; c < NUMBER_OF_PARTICLES; ++c) {
        int coverage = 1000;
        int x = rand() % coverage + (width / 2 - coverage);
        int y = rand() % coverage + (height / 2 - coverage);
        
        MassParticle particle = MassParticle(vec3(x, y, 0));
        particle.velocity = vec3(0, 0, 0);
        particle.mass = rand() % MASS + MASS;
        
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
    float ndcX = ((2.0f * point.x) / float(width)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height)) - 1.0f;
    return vec4(ndcX, ndcY, 0, 1.0);
}

void SpaceSystem::step() {
    
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
    center = vec3(width / 2, height / 2, 0) + (m * direction);
    
    for (MassParticle& p : particles) {
        vec3 direction = glm::normalize(center - p.p);
        direction *= ((16 / 600.0) * 20);
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



