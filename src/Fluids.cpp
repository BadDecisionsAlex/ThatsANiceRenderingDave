//
// Created by Joshua Cristol on 12/2/17.
//

#include "Fluids.h"


Grid::Grid(int grid_size, int dx, int dy) {
    this->N = grid_size;
    this->dx = dx;
    this->dy = dy;
    this->grid = vector<Cell>(N*N);
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            Cell& c = this->at(row, col);
            c.row = row;
            c.col = col;
            c.particle = vec2(row + (dx / 2.0f), col + (dy, 2.0f));
        }
    }
}

void FluidSystem::step() {
    //copy the current array into the old array
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    advection();
}
Cell FluidSystem::reclinearInterpolation(vec2 position){
    float halfDx = this->grid.dx / 2.0f;
    float halfDy = this->grid.dy / 2.0f;
    vec2 p1 = vec2(position.x + halfDx, position.y + halfDy);
    vec2 p2 = vec2(position.x + halfDx, position.y - halfDy);
    vec2 p3 = vec2(position.x - halfDx, position.y + halfDy);
    vec2 p4 = vec2(position.x - halfDx, position.y - halfDy);
    Cell& c1 = oldGrid.at(p1);
    Cell& c2 = oldGrid.at(p2);
    Cell& c3 = oldGrid.at(p3);
    Cell& c4 = oldGrid.at(p4);
    float d1 = glm::distance(c1.particle, position);
    float d2 = glm::distance(c2.particle, position);
    float d3 = glm::distance(c3.particle, position);
    float d4 = glm::distance(c4.particle, position);
    float sum = d1 + d2 + d2 + d4;
    float r1 = d1 / sum;
    float r2 = d2 / sum;
    float r3 = d3 / sum;
    float r4 = d4 / sum;
    Cell theoreticalCell;
    theoreticalCell.velocity = (r1 * c1.velocity) + (r2 * c2.velocity) + (r3 * c3.velocity) + (r4 * c4.velocity);
    theoreticalCell.pressure = (r1 * c1.pressure) + (r2 * c2.pressure) + (r3 * c3.pressure) + (r4 * c4.pressure);
    theoreticalCell.density = (r1 * c1.density) + (r2 * c2.density) + (r3 * c3.density) + (r4 * c4.density);
    theoreticalCell.divergence = (r1 * c1.divergence) + (r2 * c2.divergence) + (r3 * c3.divergence) + (r4 * c4.divergence);
    return theoreticalCell;
}

void FluidSystem::advection(){
    // based on the velocity vector find where the cells "particle was before in the old grid"
    // update the current quantities with the information interplated from the previous position
    for(iterator i = grid.begin(); i < grid.end(); ++i){
        Cell& currentCell = *i;
        vec2 previousParticlePosition = currentCell.particle + (-currentCell.velocity * dt);
        Cell theoreticalCell = reclinearInterpolation(previousParticlePosition);
        currentCell.velocity = theoreticalCell.velocity;
        currentCell.pressure = theoreticalCell.pressure;
        currentCell.density = theoreticalCell.density;
        currentCell.divergence = theoreticalCell.divergence;
    }
}
