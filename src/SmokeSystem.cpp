//
//  SmokeSystem.cpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#include <stdio.h>
#include "SmokeSystem.h"
#include "Shaders.h"
#include <GLFW/glfw3.h>

#define NUMBER_OF_PARTICLES 500.0

void SmokeSystem::setup() {
    //Add particles
    
    if (!EMITTER) {
        for (int c = 0; c < NUMBER_OF_PARTICLES; ++c) {
            int coverage = 500;
            int x = rand() % coverage + (width / 2 - coverage / 2);
            int y = rand() % coverage + (height / 2 - coverage / 2);
            
            MassParticle particle = MassParticle(vec3(x, y, 0));
            particle.velocity = vec3(0, 0, 0);
            particle.mass = rand() % 15 + 15;
            
            particles.push_back(particle);
        }
    }
    
    getPointsForScreen(points, velocities, types, indices);
}

void SmokeSystem::getPointsForScreen(vector<vec4>& points, vector<vec4>& velocities, vector<float>& types, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    velocities.clear();
    types.clear();
    int count = 0;
    for (MassParticle& p : particles) {
        vec4 point = toScreen(p.p, count);
        vec4 velocity = vec4(p.velocity, 1.0);
        velocity[2] = p.mass;
        points.push_back(point);
        indices.push_back(uvec1(count));
        velocities.push_back(velocity);
        
        if (EMITTER) {
            types.push_back(1);
        } else {
            types.push_back(0);
        }
        
        ++count;
    }
    vec4 point = toScreen(center, count);
    points.push_back(point);
    velocities.push_back(vec4(0, 0, 0, 1));
    types.push_back(0);
    indices.push_back(uvec1(count));
}

vec4 SmokeSystem::toScreen(const vec3& point, int id) {
    float z = -id / (float(particles.size() + 1));
    return vec4(point.x, point.y, z, 1.0);
}

void SmokeSystem::step() {
    
    stepCount += 3;
    if (stepCount >= 360) {
        stepCount = 0;
    }
    
    float r = (stepCount / 360.0) * (2 * 3.14159);
    
    vec3 c1 = vec3(cosf(r), sinf(r), 0);
    vec3 c2 = vec3(-sinf(r), cosf(r), 0);
    vec3 c3 = vec3(0, 0, 1);
    glm::mat3 m(c1, c2, c3);
    
    vec3 direction(20, 0, 0);
    
    int y_offset = 0;
    if (EMITTER) {
        y_offset = 250;
    }
    
    center = vec3(width / 2, height / 2 - y_offset, 0) + (m * direction);
    
    //Emit new particles
    if (EMITTER) {
        vector<int> toRemove;
        for (int x = 0; x < particles.size(); ++x) {
            MassParticle& p = particles[x];
            p.age += 1;
            
            if (p.age >= p.life) {
                toRemove.push_back(x);
            }
        }
        
        for (int x = toRemove.size() - 1; x >= 0; --x) {
            int index = toRemove[x];
            particles.erase(particles.begin() + index);
        }
        
        for (int x = 0; x < 3; ++x) {
            float dx = (rand() % 8) - 4;
            float dy = (rand() % 10) + 10;
            
            MassParticle newParticle(center);
            newParticle.mass = rand() % 15 + 15;
            newParticle.velocity = vec3(dx, dy, 0);
            newParticle.life = rand() % 150 + 50;
            particles.push_back(newParticle);
        }
        
        prepareDraw();
    }
    
    for (MassParticle& p : particles) {
        if (center != p.p) {
            vec3 grav = glm::normalize(center - p.p);
            grav *= ((5.333 / 600.0) * 9.807);
            p.velocity += grav;
        }
        
        if (isDragging && mouse != p.p) {
            vec3 m = glm::normalize(mouse - p.p);
//            float distance = -(5.333 / 600.0) * (fmin(fmax(0, 300 - glm::length(mouse - p.p)), 300)) / 2;
            
            float distance = (glm::length(mouse - p.p) <= 200) ? 150 : 0;
            float force_direction = mouse_button == GLFW_MOUSE_BUTTON_RIGHT ? 1.0 : -1.0;
            distance *= (5.33 / 600.0) * force_direction;
            p.velocity += m * distance;
        }
        
        p.p += p.velocity;
    }
}


//MARK: - Draw

SmokeSystem::SmokeSystem(bool emitter) : EMITTER(emitter), particle_pass(-1, particle_pass_input, { smoke_vertex_shader, smoke_geometry_shader, smoke_fragment_shader }, { /* uniforms */ }, { "fragment_color" }) {
    getPointsForScreen(points, velocities, types, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
    particle_pass_input.assign(1, "point_velocity", velocities.data(), velocities.size(), 4, GL_FLOAT);
    particle_pass_input.assign(2, "color_type", types.data(), types.size(), 1, GL_FLOAT);
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
    getPointsForScreen(points, velocities, types, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.updateVBO(1, velocities.data(), velocities.size());
    particle_pass.updateVBO(2, types.data(), types.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}


