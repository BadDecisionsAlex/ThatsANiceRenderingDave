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
    float density = 10.0;
    vec2 velocity = vec2(0.0f, 0.01f);
    float pressure = 0.0;
    float divergence = 0.0;
    vec2 particle;
};
typedef  vector<Cell>::iterator iterator;
typedef  vector<Cell>::const_iterator const_iterator;

struct Grid{
    Grid(int grid_size, int dx, int dy);
    int N;
    float dx;
    float dy;
    vector<Cell> grid;

    Cell& at( int r, int c ) { return grid[ r * N + c ]; }
    vec2 cellToParticle(int i, int j);
    iterator begin() {return grid.begin();}
    const_iterator begin() const {return grid.begin();}
    iterator end() {return grid.end();}
    const_iterator end() const {return grid.end();}
};

class FluidSystem {
public:
    FluidSystem(int grid_size, int dx, int dy, float dt) : grid(Grid(grid_size, dx, dy)), oldGrid(Grid(grid_size, dx, dy)), dt(dt){}
    void step();

private:
    Grid grid;
    Grid oldGrid;
    float dt;
    void advectVelocity();
    void advectDensity();
    Cell interpolate(vec2 position);
    Cell imaginationHelper(Cell& a, Cell& b, Cell& c, Cell& d, float rA, float rB, float rC, float rD);
    void diffuseVelocity();
    void diffuseDensity();
    void project();
};

#endif
