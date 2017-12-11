// Comment out the line below to add this bad boy to the compile chain or to see the code with proper color fields.
// Removed from compile chain until it is complete.
//#if 0

#ifndef FLUIDS_H
#define FLUIDS_H

#include "ParticleSystem.h"
#include <glm/glm.hpp>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <functional>

using glm::vec2;
using glm::ivec2;
using glm::uvec1;
using glm::vec1;
using std::vector;
using std::size_t;
using std::ostream;

// **************
// USING ALIASES
// **************
struct Cell;
struct Grid;
using Accessor = std::function<float&(Cell&)>;

// *****
// Cell
// *****

struct Cell {

    // Member Variables
    int i;                  // index, used to calculate grid coords and position
    float vx;               // velocity in horizontal, X axis
    float vy;               // velocity in vertical, Y axis
    float den;              // density
    // To avoid additional memory slots these values may become unstable inside of an operation function.
    // However they will always be stable at the end of step.

    // Class Variables
    // change printvar to change which value we print by << operator
    static Accessor printVar;

    Cell( int i_ = 0, float vx_ = 0.0f, float vy_ = 0.0f, float den_ = 0.0f ) : i(i_), vx(vx_), vy(vy_), den(den_){}

    // Functions
    ivec2 coord() const;
    vec2 pos() const;

};

// *****
// GRID
// *****

typedef  vector<Cell>::iterator iterator;
typedef  vector<Cell>::const_iterator const_iterator;

struct Grid{

    // Member Variables
    vector<Cell> grid;

    // Class Variables
    static int N;
    static float dx;
    static float dy;
    static int printSpacing; // default is 5

    Grid( int n_ = 10, float dx_ = 10, float dy_ = 10 ) : grid(vector<Cell>(1)) { N=n_; dx=dx_; dy=dy_; printSpacing=7; clear(); }

    // Functions
    //
    // Remember Coords are stored (row, column) and Positions are stored (x, y)
    // it is very easy to confuse the horizontal and vertical mapping conversion!
    //
    void clear(){
        grid = vector<Cell>( (N+2)*(N+2) );
        int i=0;
        for(Cell& c : grid)
            c.i = i++;
    }
    Cell& at( int r, int c ) { return grid[ r * (N+2) + c ]; }
    const Cell& at( int r, int c ) const { return grid[ r * (N+2) + c]; }
    //FIXME make these consistent by dependence. This is begging for errors.
    static ivec2 iToCo( int i ) { return ivec2( i / (N+2), i % (N+2)); }
    static int coToI( int r, int c ) { return r * (N+2) + c; }
    static int coToI( ivec2 v )  { return coToI( v.x, v.y ); }
    static vec2 iToPos( int i ) { return coToPos(iToCo(i)); }
    static vec2 coToPos( int r, int c ) { return vec2( (float(r) + 0.5f), (float(c) + 0.5f)); }
    static vec2 coToPos( ivec2 v ) { return coToPos( v.x, v.y ); }
    static int posToI( float x, float y ) { return coToI(int(x), int(y)); }
    static int posToI( vec2 v ){return posToI( v.x, v.y );}
    static ivec2 posToCo( float x, float y ) { return ivec2( int(x), int(y)); }
    static ivec2 posToCo( vec2 v ) { return posToCo( v.x, v.y ); }

    // STL Compliance
    iterator begin() {return grid.begin();}
    const_iterator begin() const {return grid.begin();}
    iterator end() {return grid.end();}
    const_iterator end() const {return grid.end();}

};

class FluidSystem : public ParticleSystem {
public:
    FluidSystem(int grid_size=10, int dx_=10, int dy_=10, float time_step=(1.0f/60.0f), float diff_=0.0001f, float visc_=0.0001f );

    // Draw Functions
    void printAll();
    void printCurrent();
    void printOld();
    void print(Accessor var, Grid& src);
    void test();
    void step();
    void setup();
    void getPointsForScreen(vector<vec4>& particles, vector<vec1>& densities, vector<uvec1>& indices);
    vec4 toScreen(const vec2& particle);
    void prepareDraw();
    void draw();

private:
    // Member Variables 
    Grid grid;
    Grid oldGrid;
    float dt;
    float diffusion;
    float viscosity;

    // Functions
    
    // mixes the value of var in src to var in "grid"
    void advect( Accessor var, Grid& src, short f );
    // mixes varB into varA applying scalar
    void diffuse( Accessor varA, Accessor varB, float scalar, short f );
    // corrects divergence in flows
    void project();
    // Helper for Diffuse and Project : applies a linear interpolation between neighboring cells from srcB.varB to srcA.varB using two scalars.
    void linearSolver( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB, float scalarNumerator, float scalarDenominator, short f );
    // Applies var from the body to the boundary cells of src
    void fixBoundary( Accessor var, Grid& src, short f );
    // applies force/scalar from srcB to varA in srcA and applies dt
    void add( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB );
    // moves a value from grid to oldGrid
    void update( Accessor var );
    void swap( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB ) ;

    //Rendering (Could be made simpler)
    RenderDataInput fluid_pass_input;
    RenderPass fluid_pass;
    vector<vec4> particles;
    vector<vec1> densities;
    vector<uvec1> indices;
};

#endif
