//
// Created by Joshua Cristol on 11/24/17.
//

#ifndef PARTICLES_PARTICLEGRID_H
#define PARTICLES_PARTICLEGRID_H

#include "AbstractParticle.h"   // AbstractParticle, VerletParticle
#include <glm/glm.hpp>          // vec2, vec3
#include <vector>               // vector
#include <time.h>               // needed for timing the step
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>


using glm::vec2;
using glm::vec3;
using std::pair;
using std::vector;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::set;

class ParticleGrid{
public:

    // typedefs to help access the grid
    // these typedefs should not be used for the return values of functions in the class
    typedef unordered_set<int> ParticleIdSet;
    typedef set<VerletParticle> ParticleSet;
    typedef unordered_set<VerletParticle> UnorderedParticleSet;
    typedef vector<VerletParticle> ParticleVector;
    typedef vector<ParticleVector> ParticleRow;
    typedef vector<ParticleRow> t_particle_grid;

    ParticleGrid(){}
    ParticleGrid(float n, float screen_width, float screen_height);

    vector<VerletParticle> collides(VerletParticle p);
    void update(vector<VerletParticle> v);

private:

    // private data
    t_particle_grid grid;
    unordered_map<int, VerletParticle> grid_map;
    float blockSize;
    float screen_width;
    float screen_height;
    int num_cols;
    int num_rows;

    // private methods
    vector<VerletParticle> residents(VerletParticle p);
    pair<int, int> cellPosition(VerletParticle p);
    void insert(VerletParticle p);
    void clear();
    void reduceVector(const vector<VerletParticle>& v, ParticleIdSet& ids);
    void expandVector(const ParticleIdSet& ids, vector<VerletParticle>& v);

};
#endif //PARTICLES_PARTICLEGRID_H
