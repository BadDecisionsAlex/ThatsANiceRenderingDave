//
// Created by Joshua Cristol on 11/24/17.
//

#ifndef PARTICLES_PARTICLEGRID_H
#define PARTICLES_PARTICLEGRID_H

#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <time.h>               // needed for timing the step


using glm::vec2;
using glm::vec3;
using std::vector;
using std::pair;

class ParticleGrid{
public:
    ParticleGrid(){}
    ParticleGrid(int n, float screen_width, float screen_height);
    void update(vector<VerletParticle> v);
    vector<VerletParticle> collides(VerletParticle p);

private:
    // typedef to help access the grid
    typedef std::vector<VerletParticle> VerletVector;
    typedef std::vector<VerletVector> ColVector;
    typedef std::vector<ColVector> ParticleVector;

    // private data
    ParticleVector grid;
    int blockSize;
    float screen_width;
    float screen_height;
    int dividedWidth;
    int dividedHeight;

    // private methods
    void insert(VerletParticle p);
    vector<VerletParticle> getCell(VerletParticle p);
    void clear();

};
#endif //PARTICLES_PARTICLEGRID_H
