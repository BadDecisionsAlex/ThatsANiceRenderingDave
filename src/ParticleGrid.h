//
// Created by Joshua Cristol on 11/24/17.
//

#ifndef PARTICLES_PARTICLEGRID_H
#define PARTICLES_PARTICLEGRID_H

#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <unordered_set>
#include <set>
#include <time.h>               // needed for timing the step

using std::vector;

class ParticleGrid{
public:

    // typedef to help access the grid
    typedef std::unordered_set<VerletParticle*> VerletSet;
    typedef std::vector<VerletParticle*> VerletPointerVector;
    typedef std::vector<VerletParticle> VerletVector;
    typedef std::vector<VerletSet> ColVector;
    typedef std::vector<ColVector> ParticleVector;


    ParticleGrid(){}
    ParticleGrid(int n, float screen_width, float screen_height);
    void update(vector<VerletParticle> v);
    vector<VerletParticle> collides(VerletParticle p);

private:

    // private data
    ParticleVector grid;
    int blockSize;
    float screen_width;
    float screen_height;
    int dividedWidth;
    int dividedHeight;

    // private methods
    void insert(VerletParticle p);
    VerletSet getCell(VerletParticle p);
    void clear();

};
#endif //PARTICLES_PARTICLEGRID_H
