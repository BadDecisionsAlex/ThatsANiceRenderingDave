//
//  PointGridSystemHandler.cpp
//  Particles
//
//  Created by Matthew Ruston on 12/10/17.
//

#include "PointGridSystemHandler.h"
#include "Shaders.h"

vec4 PointGridSystemHandler::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.x) / float(width)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height)) - 1.0f;
    //TODO: Pass in the count through its own buffer, not using the z value
    return vec4(ndcX, ndcY, point.z, 1.0);
}

void PointGridSystemHandler::getPointsForScreen(vector<vec4>& points, vector<uvec1>& indices) {
    points.clear();
    indices.clear();
    int count = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            
            int x = c * nodeSize + nodeSize / 2;
            int y = r * nodeSize + nodeSize / 2;
            
            int weight = grid[r][c].numberOfParticles;
            vec4 position = toScreen(vec3(x, y, weight));
            
            points.push_back(position);
            indices.push_back(uvec1(count));
            ++count;
        }
    }
}

PointGridSystemHandler::PointGridSystemHandler(SYSTEM* _system) : system(_system), particle_pass(-1, particle_pass_input, { point_grid_vertex_shader, point_grid_geometry_shader, point_grid_fragment_shader }, { /* uniforms */ }, { "fragment_color" }) {
    getPointsForScreen(points, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
}

void PointGridSystemHandler::prepareDraw() {
    particle_pass_input.assign_index(indices.data(), indices.size(), 1);
    particle_pass = RenderPass(-1,
                               particle_pass_input,
                               {
                                   point_grid_vertex_shader,
                                   point_grid_geometry_shader,
                                   point_grid_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
                               );
}

void PointGridSystemHandler::draw() {
    getPointsForScreen(points, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}
