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

class FluidSystem {
public:
    FluidSystem(int grid_size, int dx, int dy) : grid(Grid(grid_size, dx, dy)){}

private:
    // Cell definition
    struct Cell {
        float density = 0.0;
        float oldDensity = 0.0;
        vec2 velocity;
        vec2 oldVelocity;
        float pressure = 0.0;
        float divergence = 0.0;
    };
    typedef  vector<Cell>::iterator iterator;
    typedef  vector<Cell>::const_iterator const_iterator;

    // inner grid class
    class Grid{
        public:
            Grid(int grid_size, int dx, int dy) : N(grid_size), dx(dx), dy(dy){}
            int N;
            int dx;
            int dy;
            vector<Cell> grid = vector<Cell>(N*N);
            Cell& operator[] ( int i ){ return grid[i]; }
            const Cell& operator[] ( int i ) const { return grid[i]; }
            Cell& at( int r, int c ) { return grid[ r + N * c ]; }
            const Cell& at(int r, int c) const { return grid[ r + N * c ]; }
            // iterators
            iterator begin() {return grid.begin();}
            const_iterator begin() const {return grid.begin();}
            iterator end() {return grid.end();}
            const_iterator end() const {return grid.end();}
        };

    // Fluid System data
    Grid grid;
};

#endif
