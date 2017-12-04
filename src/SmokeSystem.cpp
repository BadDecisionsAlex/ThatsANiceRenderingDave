//
//  SmokeSystem.cpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#include <stdio.h>
#include "SmokeSystem.h"
#include "Shaders.h"

#define NUMBER_OF_PARTICLES 750.0

void SmokeSystem::setup() {
    //Add particles
    for (int c = 0; c < NUMBER_OF_PARTICLES; ++c) {
        int x = rand() % 500 + 750;
        int y = rand() % 500 + 750;
        
        float dx = (rand() % 1000) / 500.0 - 1;
        float dy = (rand() % 1000) / 500.0 - 1;
        
        MassParticle particle = MassParticle(vec3(x, y, 0));
        particle.velocity = vec3(0, 0, 0);
        particle.mass = rand() % 15 + 15;
        
        particles.push_back(particle);
    }
    
    getPointsForScreen(points, velocities, indices);
}

void SmokeSystem::getPointsForScreen(vector<vec4>& points, vector<vec4>& velocities, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    velocities.clear();
    int count = 0;
    for (MassParticle& p : particles) {
        vec4 point = toScreen(p.p, count);
        vec4 velocity = vec4(p.velocity, 1.0);
        velocity[2] = p.mass;
        points.push_back(point);
        indices.push_back(uvec1(count));
        velocities.push_back(velocity);
        ++count;
    }
    vec4 point = toScreen(center, count);
    points.push_back(point);
    velocities.push_back(vec4(0, 0, 0, 1));
    indices.push_back(uvec1(count));
}

vec4 SmokeSystem::toScreen(const vec3& point, int id) {
//    float ndcX = ((2.0f * point.x) / float(width * 2)) - 1.0f;
//    float ndcY = ((2.0f * point.y) / float(height * 2)) - 1.0f;
    float z = -id / (NUMBER_OF_PARTICLES + 1);
    return vec4(point.x, point.y, z, 1.0);
}

void SmokeSystem::step() {
    
    stepCount += 1;
    if (stepCount >= 360) {
        stepCount = 0;
    }

    float r = (stepCount / 360.0) * (2 * 3.14159);

    vec3 c1 = vec3(cosf(r), sinf(r), 0);
    vec3 c2 = vec3(-sinf(r), cosf(r), 0);
    vec3 c3 = vec3(0, 0, 1);
    glm::mat3 m(c1, c2, c3);

    vec3 direction(20, 0, 0);
    center = vec3(1000, 1000, 0) + (m * direction);
    
    for (MassParticle& p : particles) {
        vec3 direction = glm::normalize(center - p.p);
        direction *= ((5.333 / 600.0) * 9.807);
        p.velocity += direction;
        
        p.p += p.velocity;
    }
}


//MARK: - Draw

SmokeSystem::SmokeSystem() : particle_pass(-1, particle_pass_input, { smoke_vertex_shader, smoke_geometry_shader, smoke_fragment_shader }, { /* uniforms */ }, { "fragment_color" }) {
    getPointsForScreen(points, velocities, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
    particle_pass_input.assign(1, "point_velocity", velocities.data(), velocities.size(), 4, GL_FLOAT);
}

void SmokeSystem::prepareDraw() {
    particle_pass_input.assign_index(indices.data(), indices.size(), 1);
    particle_pass = RenderPass(-1,
                               particle_pass_input,
                               {
                                   smoke_vertex_shader,
                                   smoke_geometry_shader,
                                   smoke_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
                               );
}

void SmokeSystem::draw() {
    getPointsForScreen(points, velocities, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.updateVBO(1, velocities.data(), velocities.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}


