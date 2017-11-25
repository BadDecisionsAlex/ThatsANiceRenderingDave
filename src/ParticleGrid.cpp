//
// Created by Joshua Cristol on 11/24/17.
//

#include "ParticleGrid.h"


ParticleGrid::ParticleGrid(int blockSize, float screen_width, float screen_height){
    this->blockSize = blockSize;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    int dividedWidth = (int) screen_width / blockSize;
    int dividedHeight = (int) screen_height / blockSize;
    this->dividedWidth = dividedWidth;
    this->dividedHeight = dividedHeight;
    for (int i = 0; i < dividedWidth; ++i) {
        ColVector r;
        for (int j = 0; j < dividedHeight; ++j) {
            VerletVector s;
            r.push_back(s);
        }
        grid.push_back(r);
    }
}

void ParticleGrid::clear(){
    for (int i = 0; i < grid.size(); ++i) {
        ColVector& row = grid[i];
        for (int j = 0; j < row.size(); ++j) {
            VerletVector& v = row[j];
            v.clear();
        }
    }
}

void ParticleGrid::update(vector<VerletParticle> pVector) {
    this->clear();
    for (int i = 0; i < pVector.size(); ++i) {
        this->insert(pVector[i]);
    }
}

void ParticleGrid::insert(VerletParticle p){
    int cellX = p.pos().x / blockSize;
    int cellY = p.pos().y / blockSize;
    cellX = std::min(this->screen_width, (float)std::max(0,cellX));
    cellY = std::min(this->screen_height, (float)std::max(0,cellY));
    grid[cellX][cellY].push_back(p);
}

vector<VerletParticle> ParticleGrid::getCell(VerletParticle p){
    int cellX = p.pos().x / blockSize;
    int cellY = p.pos().y / blockSize;
    cellX = std::min(this->screen_width, (float)std::max(0,cellX));
    cellY = std::min(this->screen_height, (float)std::max(0,cellY));
    return grid[cellX][cellY];
}

vector<VerletParticle> ParticleGrid::collides(VerletParticle p){
    vector<VerletParticle> result;
    float x = p.pos().x;
    float y = p.pos().y;
    float radius = p.radius;
    vector<VerletParticle> home = getCell(VerletParticle(x , y, 0));
    vector<VerletParticle> topRight = getCell(VerletParticle(x + radius, y + radius, 0));
    vector<VerletParticle> topLeft = getCell(VerletParticle(x - radius, y + radius, 0));
    vector<VerletParticle> bottomRight = getCell(VerletParticle(x + radius, y - radius, 0));
    vector<VerletParticle> bottomLeft = getCell(VerletParticle(x - radius, y - radius, 0));
    result.insert(result.end(), home.begin(), home.end());
    if(topRight != home && topRight != topLeft && topRight != bottomLeft && topRight != bottomRight){
        result.insert(result.end(), topRight.begin(), topRight.end());
    }
    if(topLeft != home && topLeft != topRight && topLeft != bottomLeft && topLeft != bottomRight){
        result.insert(result.end(), topLeft.begin(), topLeft.end());
    }
    if(bottomRight != home && bottomRight != topLeft && bottomRight != bottomLeft && bottomRight != topRight){
        result.insert(result.end(), bottomRight.begin(), bottomRight.end());
    }
    if(bottomLeft != home && bottomLeft != topLeft && bottomLeft != topLeft && bottomLeft != topRight){
        result.insert(result.end(), bottomLeft.begin(), bottomLeft.end());
    }
    return result;
}
