//
// Created by Joshua Cristol on 12/2/17.
//

#include "Fluids.h"


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

void FluidSystem::step() {
    //copy the current array into the old array
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    advection();
}

void FluidSystem::advection(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& currentCell = grid.at(i, j);
            vec2 particleCell = grid.cellToParticle(i, j);
            vec2 backwardStepParticle = particleCell - dt * currentCell.velocity;
            Cell iCell = interpolate(backwardStepParticle);
            currentCell.divergence = iCell.divergence;
            currentCell.pressure = iCell.pressure;
            currentCell.velocity = iCell.velocity;
            currentCell.density = iCell.density;
        }
    }
}

