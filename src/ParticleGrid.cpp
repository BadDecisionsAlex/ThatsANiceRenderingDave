//
// Created by Joshua Cristol on 11/24/17.
//

#include "ParticleGrid.h"


ParticleGrid::ParticleGrid(float blockSize, float screen_width, float screen_height){
    this->blockSize = blockSize;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    this->num_cols = (int) screen_width / blockSize;
    this->num_rows = (int) screen_height / blockSize;
    for (int i = 0; i < num_rows; ++i) {
        ParticleRow r;
        for (int j = 0; j < num_cols; ++j) {
            ParticleVector s;
            r.push_back(s);
        }
        grid.push_back(r);
    }
}

void ParticleGrid::clear(){
    grid_map.clear();
    for (int i = 0; i < grid.size(); ++i) {
        ParticleRow row = grid[i];
        for (int j = 0; j < row.size(); ++j) {
            ParticleVector v = row[j];
            v.clear();
        }
    }
}

void ParticleGrid::update(vector<VerletParticle> pVector) {
    this->clear();
    for (int i = 0; i < pVector.size(); ++i) {
        pVector[i].id = i;
        this->insert(pVector[i]);
    }
}

void ParticleGrid::insert(VerletParticle p){
    pair<int, int> position = cellPosition(p);
    grid_map.insert(std::pair<int, VerletParticle>(p.id, p));
    grid[position.first][position.second].push_back(p);
}

pair<int, int> ParticleGrid::cellPosition(VerletParticle p){
    int cellX = std::floor(p.pos().x / blockSize);
    int cellY = std::floor(p.pos().y / blockSize);
    cellX = std::min(this->num_rows - 1.0f, (float)std::max(0,cellX));
    cellY = std::min(this->num_cols - 1.0f, (float)std::max(0,cellY));
    pair<int, int> position = pair<int, int>(cellX, cellY);
    return position;
}

void ParticleGrid::reduceVector(const vector<VerletParticle>& v, ParticleIdSet& ids){
    for (int i = 0; i < v.size(); ++i) {
        VerletParticle p = v[i];
       ids.insert(p.id);
    }
}

void ParticleGrid::expandVector(const ParticleIdSet& ids, vector<VerletParticle>& v){
    ParticleIdSet::iterator it;
    for (it = ids.begin(); it != ids.end(); ++it) {
        int id = *it;
        v.push_back(grid_map[id]);
    }
}

vector<VerletParticle> ParticleGrid::residents(VerletParticle p){
    pair<int, int> position = cellPosition(p);
    return grid[position.first][position.second];
}

vector<VerletParticle> ParticleGrid::collides(VerletParticle p){
    ParticleIdSet ids;
    vector<VerletParticle> home_cell = residents(p);
    float x = p.pos().x;
    float y = p.pos().y;
    float radius = p.radius;
    ParticleVector north = residents(VerletParticle(x , y + blockSize));
    ParticleVector south = residents(VerletParticle(x , y - blockSize));
    ParticleVector east = residents(VerletParticle(x + blockSize, y));
    ParticleVector west = residents(VerletParticle(x - blockSize, y));
    ParticleVector northeast = residents(VerletParticle(x + blockSize, y + blockSize));
    ParticleVector northwest = residents(VerletParticle(x - blockSize, y + blockSize));
    ParticleVector southeast = residents(VerletParticle(x + blockSize, y - blockSize));
    ParticleVector southwest = residents(VerletParticle(x - blockSize, y - blockSize));
    reduceVector(home_cell, ids);
    reduceVector(north, ids);
    reduceVector(south, ids);
    reduceVector(east, ids);
    reduceVector(west, ids);
    reduceVector(northeast, ids);
    reduceVector(northwest, ids);
    reduceVector(southeast, ids);
    reduceVector(southwest, ids);
    ids.erase(p.id);
    vector<VerletParticle> v;
    expandVector(ids, v);
    return v;
}
