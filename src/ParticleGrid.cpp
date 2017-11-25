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
            VerletSet s;
            r.push_back(s);
        }
        grid.push_back(r);
    }
}

void ParticleGrid::clear(){
    for (int i = 0; i < grid.size(); ++i) {
        ColVector& row = grid[i];
        for (int j = 0; j < row.size(); ++j) {
            VerletSet& v = row[j];
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
    grid[cellX][cellY].insert(&p);
}

ParticleGrid::VerletSet ParticleGrid::getCell(VerletParticle p){
    int cellX = p.pos().x / blockSize;
    int cellY = p.pos().y / blockSize;
    cellX = std::min(this->screen_width, (float)std::max(0,cellX));
    cellY = std::min(this->screen_height, (float)std::max(0,cellY));
    return grid[cellX][cellY];
}

vector<VerletParticle> ParticleGrid::collides(VerletParticle p){
    vector<VerletParticle> result;
    VerletPointerVector intermResult;
    VerletSet resultSet;
    float x = p.pos().x;
    float y = p.pos().y;
    float radius = p.radius;
    VerletSet home = getCell(VerletParticle(x , y, 0));
    VerletSet topRight = getCell(VerletParticle(x + radius, y + radius, 0));
    VerletSet topLeft = getCell(VerletParticle(x - radius, y + radius, 0));
    VerletSet bottomRight = getCell(VerletParticle(x + radius, y - radius, 0));
    VerletSet bottomLeft = getCell(VerletParticle(x - radius, y - radius, 0));
    resultSet.insert(home.begin(), home.end());
    if(topRight != home && topRight != topLeft && topRight != bottomLeft && topRight != bottomRight){
        resultSet.insert(topRight.begin(), topRight.end());
    }
    if(topLeft != home && topLeft != topRight && topLeft != bottomLeft && topLeft != bottomRight){
        resultSet.insert(topLeft.begin(), topLeft.end());
    }
    if(bottomRight != home && bottomRight != topLeft && bottomRight != bottomLeft && bottomRight != topRight){
        resultSet.insert(bottomRight.begin(), bottomRight.end());
    }
    if(bottomLeft != home && bottomLeft != topLeft && bottomLeft != topLeft && bottomLeft != topRight){
        resultSet.insert(bottomLeft.begin(), bottomLeft.end());
    }
    resultSet.erase(&p);
    intermResult.insert(intermResult.end(), resultSet.begin(), resultSet.end());
    for (int i = 0; i < intermResult.size(); ++i) {
        result.push_back(*intermResult[i]);
    }
    return result;
}
