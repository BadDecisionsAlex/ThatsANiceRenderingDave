//
// Created by Joshua Cristol on 12/2/17.
//

#include "Fluids.h"
#include "Shaders.h"


Grid::Grid(int grid_size, int dx, int dy) {
    this->N = grid_size;
    this->dx = dx;
    this->dy = dy;
    this->grid = vector<Cell>((N + 2) * (N + 2));
    for (int i = 1; i < N + 1; ++i) {
        for (int j = 1; j < N + 1; ++j) {
            Cell& c = this->at(i, j);
            c.particle = cellToParticle(i, j);
        }
    }
}

vec2 Grid::cellToParticle(int i, int j){
    return vec2(i * (dx / 2.0f), j * (dy / 2.0f));
}

Cell FluidSystem::imaginationHelper(Cell& a, Cell& b, Cell& c, Cell& d, float rA, float rB, float rC, float rD){
    Cell imaginationCell;
    imaginationCell.velocity = a.velocity * (1.0f / rA) + b.velocity * (1.0f / rB) + c.velocity * (1.0f / rC) + d.velocity * (1.0f / rD);
    imaginationCell.density = a.density * (1.0f / rA) + b.density * (1.0f / rB) + c.density * (1.0f / rC) + d.density * (1.0f / rD);
    imaginationCell.pressure = a.pressure * (1.0f / rA) + b.pressure * (1.0f / rB) + c.pressure * (1.0f / rC) + d.pressure * (1.0f / rD);
    imaginationCell.divergence = a.divergence * (1.0f / rA) + b.divergence * (1.0f / rB) + c.divergence * (1.0f / rC) + d.divergence * (1.0f / rD);
    return imaginationCell;
}

Cell FluidSystem::interpolate(vec2 position){
    int cellX = (position.x / oldGrid.dx) + 1;
    int cellY = (position.y / oldGrid.dy) + 1;
    Cell& residentCell = oldGrid.at(cellX, cellY);
    Cell& north = oldGrid.at(cellX, cellY - 1);
    Cell& south = oldGrid.at(cellX, cellY + 1);
    Cell& east = oldGrid.at(cellX + 1, cellY);
    Cell& west = oldGrid.at(cellX - 1, cellY);
    Cell& northWest = oldGrid.at(cellX - 1, cellY - 1);
    Cell& northEast = oldGrid.at(cellX + 1, cellY - 1);
    Cell& southWest = oldGrid.at(cellX - 1, cellY + 1);
    Cell& southEast = oldGrid.at(cellX + 1, cellY + 1);
    //interpolate from this cells position to the other ones find ratios basically
    float distResidentCell = glm::distance(residentCell.particle, position);
    if(distResidentCell == 0.0f){
        return residentCell;
    }
    // position is (right or horizontally inline)  and (below or vertically inline) the middle bottom right quad
    Cell imaginationCell;
    if(position.x >= cellX && position.y >= cellY){
        float distEast = glm::distance(east.particle, position);
        float distSouthEast = glm::distance(southEast.particle, position);
        float distSouth = glm::distance(south.particle, position);
        float sum = distEast + distSouthEast + distSouth + distResidentCell;
        float ratioEast = distEast / sum;
        float ratioSouthEast = distSouthEast / sum;
        float ratioSouth = distSouth / sum;
        float ratioResident = distResidentCell / sum;
        return imaginationHelper(east, southEast, south, residentCell, ratioEast, ratioSouthEast, ratioSouth, ratioResident);
    }
    // position is (right or horizontally inline)  and (above) the middle top right quad
    if(position.x >= cellX && position.y < cellY){
        float distEast = glm::distance(east.particle, position);
        float distNorthEast = glm::distance(northEast.particle, position);
        float distNorth = glm::distance(north.particle, position);
        float sum = distEast + distNorthEast + distNorth + distResidentCell;
        float ratioEast = distEast / sum;
        float ratioNorthEast = distNorthEast / sum;
        float ratioNorth = distNorth / sum;
        float ratioResident = distResidentCell / sum;
        return imaginationHelper(east, northEast, north, residentCell, ratioEast, ratioNorthEast, ratioNorth, ratioResident);
    }
    // position is (left)  and (below or vertically inline) the middle bottom left quad
    if(position.x < cellX && position.y >= cellY){
        float distWest = glm::distance(west.particle, position);
        float distSouthWest = glm::distance(southWest.particle, position);
        float distSouth = glm::distance(south.particle, position);
        float sum = distWest + distSouthWest + distSouth + distResidentCell;
        float ratioWest = distWest / sum;
        float ratioSouthWest = distSouthWest / sum;
        float ratioSouth = distSouth / sum;
        float ratioResident = distResidentCell / sum;
        return imaginationHelper(west, southWest, south, residentCell, ratioWest, ratioSouthWest, ratioSouth, ratioResident);
    }
    // position is (left)  and (above) the middle top left quad
    if(position.x < cellX && position.y < cellY){
        float distWest = glm::distance(west.particle, position);
        float distNorthWest = glm::distance(northWest.particle, position);
        float distNorth = glm::distance(north.particle, position);
        float sum = distWest + distNorthWest + distNorth + distResidentCell;
        float ratioWest = distWest / sum;
        float ratioNorthWest = distNorthWest / sum;
        float ratioNorth = distNorth / sum;
        float ratioResident = distResidentCell / sum;
        return imaginationHelper(west, northWest, north, residentCell, ratioWest, ratioNorthWest, ratioNorth, ratioResident);
    }
}

void FluidSystem::advectVelocity(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& currentCell = grid.at(i, j);
            vec2 particleCell = grid.cellToParticle(i, j);
            vec2 backwardStepParticle = particleCell - dt * currentCell.velocity;
            Cell iCell = interpolate(backwardStepParticle);
            currentCell.velocity = iCell.velocity;
        }
    }
}

void FluidSystem::advectDensity(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& currentCell = grid.at(i, j);
            vec2 particleCell = grid.cellToParticle(i, j);
            vec2 backwardStepParticle = particleCell - dt * currentCell.velocity;
            Cell iCell = interpolate(backwardStepParticle);
            currentCell.density= iCell.density;
        }
    }
}

void FluidSystem::diffuseVelocity(){
    float viscoity = grid.N * grid.N * 0.2;
    for (int k = 0; k < 20; ++k) {
        for (int i = 1; i < grid.N + 1; ++i) {
            for (int j = 1; j < grid.N + 1; ++j) {
                Cell& currentCell = grid.at(i, j);
                Cell& north = oldGrid.at(i, j - 1);
                Cell& south = oldGrid.at(i, j + 1);
                Cell& east = oldGrid.at(i + 1, j);
                Cell& west = oldGrid.at(i - 1, j);
                //make left hand just horizonatal component same for the right hand
                vec2 leftHandLaplace = (east.velocity - (2.0f * currentCell.velocity) + west.velocity) / (grid.dx * grid.dx);
                vec2 rightHandLaplace = (south.velocity - (2.0f * currentCell.velocity) + north.velocity) / (grid.dy * grid.dy);
                vec2 laplace = (leftHandLaplace + rightHandLaplace) * viscoity;
                currentCell.velocity = laplace;
            }
        }
    }
}

void FluidSystem::diffuseDensity(){
    float viscoity = grid.N * grid.N * 0.2;
    for (int k = 0; k < 20; ++k) {
        for (int i = 1; i < grid.N + 1; ++i) {
            for (int j = 1; j < grid.N + 1; ++j) {
                Cell& currentCell = grid.at(i, j);
                Cell& north = oldGrid.at(i, j - 1);
                Cell& south = oldGrid.at(i, j + 1);
                Cell& east = oldGrid.at(i + 1, j);
                Cell& west = oldGrid.at(i - 1, j);
                //make left hand just horizonatal component same for the right hand
                float leftHandLaplace = (east.density - (2.0f * currentCell.density) + west.density) / (grid.dx * grid.dx);
                float rightHandLaplace = (south.density - (2.0f * currentCell.density) + north.density) / (grid.dy * grid.dy);
                float laplace = (leftHandLaplace + rightHandLaplace) * viscoity;
                currentCell.density = laplace;
                currentCell.pressure = currentCell.density / (grid.dx * grid.dy);
            }
        }
    }
}

void FluidSystem::project(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            // compute the gradient between each neighbors pressure
            Cell& currentCell = grid.at(i, j);
            Cell& north = oldGrid.at(i, j - 1);
            Cell& south = oldGrid.at(i, j + 1);
            Cell& east = oldGrid.at(i + 1, j);
            Cell& west = oldGrid.at(i - 1, j);
            vec2 southGrad = vec2(0.0f, currentCell.pressure - south.pressure);
            vec2 northGrad = vec2(0.0f, currentCell.pressure - north.pressure);
            vec2 eastGrad = vec2(currentCell.pressure - east.pressure, 0.0f);
            vec2 westGrad = vec2(currentCell.pressure - west.pressure, 0.0f);
            vec2 avgGrad = (southGrad + northGrad + eastGrad + westGrad) / 4.0f;
            currentCell.velocity = currentCell.velocity - avgGrad;
        }

    }
}

// drawing stuff

FluidSystem::FluidSystem(int grid_size, int dx, int dy, float time_step) : grid(Grid(grid_size, dx, dy)), oldGrid(Grid(grid_size, dx, dy)), dt(time_step), fluid_pass(-1, fluid_pass_input, {particle_vertex_shader, fluid_geometry_shader, particle_fragment_shader}, {/*uniforms*/}, {"fragment_color"}){
                    getPointsForScreen(particles, densities, indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    fluid_pass_input.assign(1, "density", densities.data(), densities.size(), 1, GL_FLOAT);
                }

void FluidSystem::prepareDraw() {
    fluid_pass_input.assign_index(indices.data(), indices.size(), 1);
    fluid_pass = RenderPass(-1,
                               fluid_pass_input,
                               {
                                       particle_vertex_shader,
                                       fluid_geometry_shader,
                                       particle_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
    );
}

void FluidSystem::draw() {
    getPointsForScreen(particles, densities, indices);
    fluid_pass.updateVBO(0, particles.data(), particles.size());
    fluid_pass.updateVBO(1, densities.data(), densities.size());
    fluid_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}

void FluidSystem::getPointsForScreen(vector<vec4>& particles, vector<vec1>& densities, vector<uvec1>& indices) {
    particles.clear();
    densities.clear();
    indices.clear();
    int count = 0;
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& c = grid.at(i, j);
            vec4 particle = toScreen(c.particle);
            particles.push_back(particle);
            densities.push_back(vec1(c.density));
            indices.push_back(uvec1(count));
            ++count;
        }
    }
}

vec4 FluidSystem::toScreen(const vec2& point) {
    float ndcX = ((2.0f * point.x) / float(width * 2)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height * 2)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0, 1.0);
}

void FluidSystem::step() {
    //copy the current array into the old array
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    // get force input from ui
    float ui_input_density;
    vec2 ui_input_velocity;
    diffuseVelocity();
    project();
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    advectVelocity();
    project();
    diffuseDensity();
    advectDensity();
}

void FluidSystem::setup() {
    // give things an intial density
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& c = grid.at(i, j);
            // give intial parameters
            c.density = 10.0f;
            c.velocity = vec2(0.0f, 0.001f);
        }
    }
    getPointsForScreen(particles, densities, indices);
}
