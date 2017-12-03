// Comment out the line below to add this bad boy to the compile chain or to see the code with proper color fields.
// Removed from compile chain until it is complete. 
//#if 0

#ifndef FLUIDS_H
#define FLUIDS_H

#include <glm/glm.hpp>
#include "ParticleSystem.h"
#include <vector>


using glm::vec2;
using std::vector;

// Cell definition
struct Cell {
    float density = 0.0;
    vec2 velocity = vec2(0.0f, 0.0f);
    float pressure = 0.0;
    float divergence = 0.0;
    vec2 particle;
    int row;
    int col;
};
typedef  vector<Cell>::iterator iterator;
typedef  vector<Cell>::const_iterator const_iterator;


class FluidSystem {
public:
    FluidSystem(int grid_size, int dx, int dy, float dt) : grid(Grid(grid_size, dx, dy)), oldGrid(Grid(grid_size, dx, dy)), dt(dt){}
    void step();

private:
    Grid grid;
    Grid oldGrid;
    float dt;
    void advection();
    Cell reclinearInterpolation(vec2 position);
};

#endif
