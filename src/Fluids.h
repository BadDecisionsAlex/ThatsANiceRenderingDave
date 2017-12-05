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
    float p;                // pressure
    float div;              // divergence
    float dist;

    // Class Variables
    // change printvar to change which value we print by << operator
    static Accessor printVar;

    Cell( int i_ = 0, float vx_ = 0.0f, float vy_ = 0.0f, float den_ = 10.0f, float p_ = 0.0f, float div_ = 0.0f ) : i(i_), vx(vx_), vy(vy_), den(den_), p(p_), div(div_){}

    // Functions
    ivec2 coord() const;
    vec2 pos() const;
    #define centerPosition() pos()

};

// *********
// Accessor
// *********

// Accessors allow us to easity pass the arguments we wish to change into a function.
//
// lets see some examples.
// normally we can access or change the value member variable den by writing :
// ** // my_cell.den = 5;
// Using accessors we will be able to also do this by saying :
// ** // _den( my_cell ) = 5;
// or by using the more convenient accessor alias :
// ** // density( my_cell ) = 5;
//
// This may seem trivial but this will be incedibly useful for making many of our functions act
// on different sets of member variables in our grids without rewriting redundant functions.
//
// For instance lets say we want to update a set of member variables between grid and oldGrid.
// simply call :
// ** // FluidSystem::update( FluidSystem&, density );
//
// ---------
// Reminder :  Accessor = std::function<float&(Cell&)>;
// ---------

Accessor _vx( &Cell::vx );              // _vx
const Accessor& velocityX(_vx);        // velocityX
Accessor _vy( &Cell::vy );              // _vy
const Accessor& velocityY(_vy);         // velocityY
Accessor _den( &Cell::den );            // _den
const Accessor& density(_den);          // density
Accessor _p( &Cell::p );                // _p
const Accessor& pressure(_p);           // pressure
Accessor _div( &Cell::div );            // _div
const Accessor& divergence(_div);       // divergence

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
    static int printSpacing; // default is 3

    Grid( int n_ = 50, float dx_ = 10, float dy_ = 10 ) : grid(vector<Cell>( (N+2) * (N+2) )) { N=n_; dx=dx_; dy=dy_; printSpacing=3; }

    // Functions
    Cell& at( int r, int c ) { return grid[ r * N + c ]; }
    const Cell& at( int r, int c ) const { return grid[ r * N + c]; }
    static ivec2 iToCo( int i ) { return ivec2( i / N, i % N); }
    #define indexToCoordinates(x) iToCo(x)
    static int coToI( int r, int c ) { return r * N + c; }
    static int coToI( ivec2 v )  { return coToI( v.x, v.y ); }
    #define coordinatesToIndex(...) { coToI(__VA_ARGS__); }
    static vec2 iToPos( int i ) { return vec2( float( i / N ) * dx + 0.5f, float( i % N ) * dy + 0.5f ); }
    static vec2 coToPos( int r, int c ) { return vec2( float(r) * dx + 0.5f, float(c) * dy + 0.5f ); }
    static vec2 coToPos( vec2 v ) { return coToPos( v.x, v.y ); }
    #define cellToParticle(...) { coToPos(__VA_ARGS__); }
    static int posToI( float x, float y ) { return int(x/dx) * N + int(y/dy); }
    static int posToI( vec2 v ){return posToI( v.x, v.y );}
    #define positionToIndex(...) { posToI(__VA_ARGS__); }
    static ivec2 posToCo( float x, float y ) { return vec2( x/dx,y/dy ); }
    static ivec2 posToCo( vec2 v ) { return posToCo( v.x, v.y ); }
    #define positionToCoordinates(...) { posToCo(__VA_ARGS__); }

    // STL Compliance
    iterator begin() {return grid.begin();}
    const_iterator begin() const {return grid.begin();}
    iterator end() {return grid.end();}
    const_iterator end() const {return grid.end();}

};

class FluidSystem : public ParticleSystem {
public:
    FluidSystem(int grid_size, int dx_, int dy_, float time_step);

    // Draw Functions
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

    // Functions
    void advect();
    void interpolate( vec2 pos, Accessor var );
    void diffuse( Accessor var );
    void project( Accessor var );
    void linearSolver( Accessor, float scalarNumerator, float scalarDenominator );
    void fixBoundary( Accessor var );
    void update( Accessor var ); // moves a value from grid to oldGrid
    void swap( Accessor var ) ;
    Cell imaginationHelper( Cell& a, Cell& b, Cell& c, Cell& d, float rA, float rB, float rC, float rD );

    //Rendering (Could be made simpler)
    RenderDataInput fluid_pass_input;
    RenderPass fluid_pass;
    vector<vec4> particles;
    vector<vec1> densities;
    vector<uvec1> indices;
};

#endif
