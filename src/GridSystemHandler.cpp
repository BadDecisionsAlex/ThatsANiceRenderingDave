//
//  GridSystemHandler.cpp
//  Particles
//
//  Created by Matthew Ruston on 12/10/17.
//

#include "GridSystemHandler.h"
#include "Shaders.h"

vec4 GridSystemHandler::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.x) / float(width)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height)) - 1.0f;
    //TODO: Pass in the count through its own buffer, not using the z value
    return vec4(ndcX, ndcY, point.z, 1.0);
}

void GridSystemHandler::getPointsForScreen(vector<vec4>& points, vector<uvec3>& indices) {
    points.clear();
    indices.clear();
//    int count = 0;
//    for (int r = 0; r < rows; ++r) {
//        for (int c = 0; c < columns; ++c) {
//
//            int x = c * nodeSize;
//            int y = r * nodeSize;
//
//            bool inBounds = true;
//            int weight = grid[r][c].numberOfParticles;
//            vec4 position1 = toScreen(vec3(x, y, weight));
//
//            inBounds = c + 1 < columns;
//            weight = inBounds ? grid[r][c + 1].numberOfParticles : 0;
//            vec4 position2 = toScreen(vec3(x + nodeSize, y, weight));
//
//            inBounds = c + 1 < columns && r + 1 < rows;
//            weight = inBounds ? grid[r + 1][c + 1].numberOfParticles : 0;
//            vec4 position3 = toScreen(vec3(x + nodeSize, y + nodeSize, weight));
//
//            inBounds = r + 1 < rows;
//            weight = inBounds ? grid[r + 1][c].numberOfParticles : 0;
//            vec4 position4 = toScreen(vec3(x, y + nodeSize, weight));
//
//            points.push_back(position1);
//            points.push_back(position2);
//            points.push_back(position3);
//            points.push_back(position4);
//            indices.push_back(uvec3(count, count + 1, count + 2));
//            indices.push_back(uvec3(count, count + 2, count + 3));
//            count += 4;
//        }
//    }
    
    Grid& grid = system->grid;
    int count = 0;
    nodeSize = float(width) / float(Grid::N);
    for (int r = 0; r < Grid::N+2; ++r) {
        for (int c = 0; c < Grid::N+2; ++c) {
            int x = c * nodeSize;
            int y = r * nodeSize;
            
            bool inBounds = true;
            float weight = grid.at(r, c).den;
            vec4 position1 = toScreen(vec3(x, y, weight));
            
            inBounds = c + 1 < Grid::N+2;
            weight = inBounds ? grid.at(r, c + 1).den : 0;
            vec4 position2 = toScreen(vec3(x + nodeSize, y, weight));
            
            inBounds = c + 1 < Grid::N+2 && r + 1 < Grid::N+2;
            weight = inBounds ? grid.at(r + 1, c + 1).den : 0;
            vec4 position3 = toScreen(vec3(x + nodeSize, y + nodeSize, weight));
            
            inBounds = r + 1 < Grid::N+2;
            weight = inBounds ? grid.at(r + 1, c).den : 0;
            vec4 position4 = toScreen(vec3(x, y + nodeSize, weight));
            
            points.push_back(position1);
            points.push_back(position2);
            points.push_back(position3);
            points.push_back(position4);
            indices.push_back(uvec3(count, count + 1, count + 2));
            indices.push_back(uvec3(count, count + 2, count + 3));
            count += 4;
        }
    }
            
}

GridSystemHandler::GridSystemHandler(SYSTEM* _system) : system(_system), particle_pass(-1, particle_pass_input, { grid_vertex_shader, grid_geometry_shader, grid_fragment_shader }, { /* uniforms */ }, { "fragment_color" }) {
    getPointsForScreen(points, indices);
    particle_pass_input.assign(0, "vertex_position", points.data(), points.size(), 4, GL_FLOAT);
}

void GridSystemHandler::prepareDraw() {
    particle_pass_input.assign_index(indices.data(), indices.size(), 3);
    particle_pass = RenderPass(-1,
                               particle_pass_input,
                               {
                                   grid_vertex_shader,
                                   grid_geometry_shader,
                                   grid_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
                               );
}

void GridSystemHandler::draw() {
    getPointsForScreen(points, indices);
    particle_pass.updateVBO(0, points.data(), points.size());
    particle_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0));
}
