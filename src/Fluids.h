// Comment out the line below to add this bad boy to the compile chain or to see the code with proper color fields.
// Removed from compile chain until it is complete. 
#if 0

#ifndef FLUIDS_H
#define FLUIDS_H

#include <glm/glm.hpp>
#include "ParticleSystem.h"
#include <vector>
using glm::vec2;
using std::vector;

#define DEFAULT_SIZE_N = 50;

// Needs to extend ParticleSystem.
// Needs an entirely new kind of drawing method than we had in Particles.
class FluidGrid {

    struct Cell {
        float d = 0.0;    // density
        float dp = 0.0;   // previous density
        vec2 v;         // velocity
        vec2 vp;        // previous velocity
    }; 
    
    // just a glorified vector<vector<Cell>> that I optimized for speed.
    struct Grid {

        typedef typename vector<Cell>::iterator iterator;
        typedef typename vector<Cell>::const_iterator const_iterator;
        
        int N = int(DEFAULT_SIZE_N);
        vector<Cell> grid = vector<Cell>(N*N);

        // speedhack non-sense to let us stuff a matrix into a vector cheaply.
        Cell& operator[] ( int r, int c ){
            return grid[r+N*c];
        }
        const Cell& operator[] ( int r, int c ) const {
            return grid[r+N*c];
        }

        // these allow us to use foreach loops
        iterator begin() {return grid.begin();}
        const_iterator begin() const {return grid.begin();}
        const_iterator cbegin() const {return grid.cbegin();}
        iterator end() {return grid.end();}
        const_iterator end() const {return grid.end();}
        const_iterator cend() const {return grid.cend();}
    };

    int N = DEFAULT_SIZE_N;     // Row/Col size of our grid
    Grid grid;

    // applies a force to all of the cells of the grid ( useful for gravity )
    void add ( vec2 force, float dt ) {
        for(Cell& c : grid)
            c.v += dt * force;
    }

    void diffuse ( float diff, float dt) {
        float a = dt * diff * N * N;
        for( int k=0; k < 20; ++k ){
            for( int r=0; r < N; ++r ){
                for( int c=0; c < N; ++c ){
                    
                    grid[r,c].v = ( 1.0 / ( 1.0 + 4.0 * a ) )                                         // we must divide the sum of our later terms by this scalar to relax them. See : Gauss-Seidel Relaxation
                        * ( grid[r,c].vp +                                                            // our previous velocity
                        a * ( grid[r-1,c].vp + grid[r+1,c].vp + grid[r,c-1].vp + grid[r,c+1].vp );    // the sum of the velocities of our neighbors
                }}}
    }

    // the psuedo code was pretty easy to parse into C++ until this point.
    // this is a near copy of their version until I can understand it more and swap for better structures and library functions. 
    // FIXME
    void advect ( float dt ) {
        int r0, c0, r1, c1;         // i have no idea what these are.
        float s0, t0, s1, t1;       // these either.
        float dt0 = dt * float(N);
        for( int r=0; r < N; ++r ){
            for( int c=0; c < N; ++c ){
                vec2 pos = vec2( r, c );
                pos -=  dt0 * grid[r,c].v;
                //  clamp between ( 0.5, N + 0.5 )
                if( pos.x < 0.5 )
                    pos.x = 0.5;
                if( pox.x > N + 0.5 )
                    pos.x = N + 0.5;
                if( pos.y < 0.5 )
                    pos.y = 0.5;
                if( pos.y > N + 0.5 )
                    pos.y = N + 0.5;
                // I think this is a really really OG round operation
                r0 = pos.x;
                r1 = r0 + 1;
                c0 = pos.y;
                c1 = c0 + 1;

                s1 = pos.x - short(r0);
                s0 = 1.0 - s1;
                t1 = pos.y - short(c0);
                t0 = 1.0 - t1;

                // set our new density from the imagined previous timestep positions of particles that are now in this cell.
                grid[r,c].d = s0 * ( t0 * grid[r0,c0].dp + t1 * grid[r0,c1].dp + s1 * grid[r1,c0].dp + t1 * grid[r1,c1].dp );
            }}
    }

    // definitely make those inputs static to optimize but I'm leaving them for now. 
    // FIXME
    void density_step( float diff, float dt ){
        add( vec2( 0.0, -9.8 ), dt );
        for( Cell& c : grid )   // put current velocity into previous velocity.
            c.vp = c.v;
        diffuse( diff, dt );
        for( Cell& c : grid )   // put current velocity into previous velocity.
            c.vp = c.v;
        advect( dt );
    }
    
    // divergence parameter shows how velocity fields move "along themselves" flowing through the grid.
    void project( float div ){
        // FIXME
    }

    void velocity_step( float visc, float dt ){
        // FIXME
    }
    
}
#endif
