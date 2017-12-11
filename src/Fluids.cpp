//
// JoShuA CrIsToL wAs HiGh OcTaNe On 12/2/17.
//

#ifndef FLUIDS_CPP
#define FLUIDS_CPP

#include <glm/gtx/string_cast.hpp>
#include "Fluids.h"
#include <queue>
#include <string>
#include "Shaders.h"
using std::function;

// *******************
// Initialize Statics
// *******************
// Set these values in the Grid header's constructor.
int Grid::N = -1;
float Grid::dx = -1;
float Grid::dy = -1;
int Grid::printSpacing = -1;

// *****
// Cell
// *****

ivec2 Cell::coord() const { return Grid::iToCo(i); }
vec2 Cell::pos() const { return Grid::iToPos(i); }
Accessor velocityX( &Cell::vx );           // _vx
Accessor velocityY( &Cell::vy );           // _vy
Accessor density( &Cell::den );            // _den
Accessor pressure( &Cell::vx );            // _p           //!// Lives temporarilty in .vx always pairs with oldGrid
Accessor divergence( &Cell::vy );          // _div         //!// Lives temporarilty in .vy always pairs with oldGrid

Accessor Cell::printVar = density;

// *****
// Grid
// *****

// Set Grid::printSpacing (int)     and     Cell::printVar (Accessor)   to change output.
std::ostream& operator<<( std::ostream& os, Grid& g ){
    std::ostringstream ss;
    ss << std::fixed;
    ss << std::setprecision(1);
    char b = ' ';
    char a = ' ';
    ss << std::setfill(b);
    for( int r=0; r<g.N+2; ++r ){
        for( int c=0; c<g.N+2; ++c ){
            if(c==1||c==g.N)
                ss << std::setfill(b);
            else if(c==1 && r != 0 && r != 1 && r != g.N && r != g.N+1)
                ss << std::setfill(a);
            ss << std::setw( Grid::printSpacing ) << Cell::printVar( g.at(r,c) );
        }
        ss << "\n\n";
    }
    return os << ss.str();
}


// ************
// FluidSystem
// ************

void FluidSystem::update( Accessor var ){
    for(int r=0; r < Grid::N+2; ++r)
        for(int c=0; c < Grid::N+2; ++c)
            var(oldGrid.at(r,c)) = var(grid.at(r,c));
}

void FluidSystem::swap( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB ){
    //std::cout << "Swap." << std::endl;
    float a,b;
    for(int r=0; r < Grid::N+2; ++r)
        for(int c=0; c < Grid::N+2; ++c){
            a = varA(srcA.at(r,c));
            b = varB(srcB.at(r,c));
            varB(srcB.at(r,c)) = a;
            varA(srcA.at(r,c)) = b;
        }
}

void FluidSystem::add( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB ){
    //std::cout << "Add." << std::endl;
    for(int r=0; r < Grid::N+2; ++r )
        for(int c=0; c < Grid::N+2; ++c)
            varA( srcA.at(r,c) ) += dt * varB( srcB.at(r,c) );
}

void FluidSystem::linearSolver( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB, float scalarNumerator, float scalarDenominator, short f ){
    //std::cout << "Linear Solver." << std::endl;
   for( int k=0; k<20; ++k ){
        for( int r=1; r < Grid::N+1; ++r ){
            for( int c=1; c < Grid::N+1; ++c ){
                varA(srcA.at(r,c)) = ( varB(srcB.at(r,c)) + scalarNumerator * ( varA(srcA.at(r-1,c)) + varA(srcA.at(r+1,c)) + varA(srcA.at(r,c-1)) + varA(srcA.at(r,c+1)) )) / scalarDenominator;
            }}
        fixBoundary( varA, srcA, f );}
}

void FluidSystem::diffuse( Accessor varA, Accessor varB, float scalar, short f ){
    //std::cout << "Diffuse." << std::endl;
    float scalarNumerator = dt * scalar * Grid::N * Grid::N;
    linearSolver( varA, varB, grid, oldGrid, scalarNumerator, (1.0f + 4.0f * scalarNumerator), f );
}

void FluidSystem::advect( Accessor var, Grid& src, short f ){
    //std::cout << "Advect." << std::endl;
    float dt0 = dt * Grid::N;
    for( int r=1; r < Grid::N+1; ++r ){
        for( int c=1; c < Grid::N+1; ++c ){
            Cell& currentCell = src.at(r,c);
            vec2 backStep = vec2( float(r) - dt0 * currentCell.vx, float(c) - dt0 * currentCell.vy );
            // check bounds
            float upper = Grid::N + 0.5f;
            if( backStep.x < 0.5f ) backStep.x = 0.5f;
            if( backStep.y < 0.5f ) backStep.y = 0.5f;
            if( backStep.x > upper ) backStep.x = upper;
            if( backStep.y > upper ) backStep.y = upper;
            
            int br = int(backStep.x);
            int bc = int(backStep.y);    
            float weightWest = backStep.x - float(br);
            float weightEast = 1.0f - weightWest;
            float weightNorth = backStep.y - float(bc);
            float weightSouth = 1.0f - weightNorth;
            
            var(grid.at(r,c)) = weightEast * ( weightSouth * var(oldGrid.at(br, bc)) + weightNorth * var(oldGrid.at(br, bc+1)) ) + weightWest * ( weightSouth * var(oldGrid.at(br+1, bc)) + weightNorth * var(oldGrid.at(br+1, bc+1)));
        }}
    fixBoundary( var, grid, f );
}

void FluidSystem::project(){
    //std::cout << "Project." << std::endl;
    for (int r=1; r < Grid::N+1; ++r) {
        for (int c=1; c < Grid::N+1; ++c) {
            // compute the gradient between each neighbors pressure
            // NOTE: I am using .vx and .vy in oldCell as a cache for pressure data
            // we no longer need those old positions at this point and this will allow us to use 2 fewer floats for each Cell.
            
            Cell& oldCell = grid.at(r, c);
            oldCell.vy = -0.5f * (grid.at(r+1,c).vx - grid.at(r-1,c).vx + grid.at(r,c+1).vy - grid.at(r,c-1).vy) / Grid::N;
            oldCell.vx = 0.0f;
            }}
    fixBoundary( divergence, oldGrid, 0 );
    fixBoundary( pressure, oldGrid, 0 );
    // dont forget we are temporarily occupying oldGrid.vx & .vy here
    linearSolver( pressure, divergence, oldGrid, oldGrid, 1.0f, 4.0f, 0 );
    for( int r=1; r < Grid::N+1; ++r ){
        for( int c=1; c < Grid::N+1; ++c ){
            Cell& currentCell = grid.at(r,c);
            currentCell.vx -= -0.5f * Grid::N * ( oldGrid.at(r+1,c).vx - oldGrid.at(r-1,c).vx );
            currentCell.vy -= -0.5f * Grid::N * ( oldGrid.at(r,c+1).vx - oldGrid.at(r,c-1).vx );
        }}
    fixBoundary( velocityX, grid, 1 );
    fixBoundary( velocityY, grid, 2 );
}

void FluidSystem::fixBoundary( Accessor accessor, Grid& src, short f ){
    //std::cout << "Fix Boundary." << std::endl;
    for( int i=1; i<=Grid::N; ++i){
       accessor( src.at( 0, i )) = (f==2? -1.0f : 1.0f) * accessor( src.at( 1, i ));
       accessor( src.at( Grid::N+1, i )) = (f==2? -1.0f : 1.0f) * accessor( src.at( Grid::N, i ));
       accessor( src.at( i, 0 )) = (f==1? -1.0f : 1.0f) * accessor( src.at( i, 1 ));
       accessor( src.at( i, Grid::N+1 )) = (f==1? -1.0f : 1.0f) * accessor( src.at( i, Grid::N ));
    }
   accessor( src.at( 0, 0 )) = 0.5f * ( accessor( src.at( 1, 0 )) + accessor( src.at( 0, 1 )));
   accessor( src.at( 0, Grid::N+1 )) = 0.5f * ( accessor( src.at( 1, src.N+1 )) + accessor( src.at( 0, Grid::N )));
   accessor( src.at( Grid::N+1, 0 )) = 0.5f * ( accessor( src.at( Grid::N, 0 )) + accessor( src.at(Grid::N+1, 1 )));
   accessor( src.at( Grid::N+1, Grid::N+1 )) = 0.5f * ( accessor( src.at( Grid::N, Grid::N+1 )) + accessor( src.at( Grid::N+1, Grid::N )));
}

// drawing stuff

FluidSystem::FluidSystem(int grid_size, int dx_, int dy_, float time_step, float diff_, float visc_ ) : grid(Grid(grid_size, dx_, dy_)), oldGrid(Grid(grid_size, dx_, dy_)), dt(time_step), diffusion(diff_), viscosity(visc_), fluid_pass(-1, fluid_pass_input, {fluid_vertex_shader, fluid_geometry_shader, particle_fragment_shader}, {/*uniforms*/}, {"fragment_color"}){
                    getPointsForScreen(particles, densities, indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    fluid_pass_input.assign(1, "density", densities.data(), densities.size(), 1, GL_FLOAT);
                }

void FluidSystem::prepareDraw() {
    //std::cout << "Prepare Draw." << std::endl;
    fluid_pass_input.assign_index(indices.data(), indices.size(), 1);
    fluid_pass = RenderPass(-1,
                               fluid_pass_input,
                               {
                                       fluid_vertex_shader,
                                       fluid_geometry_shader,
                                       particle_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
    );
}

void FluidSystem::draw() {
    //std::cout << "draw." << std::endl;
    getPointsForScreen(particles, densities, indices);
    fluid_pass.updateVBO(0, particles.data(), particles.size());
    fluid_pass.updateVBO(1, densities.data(), densities.size());
    fluid_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}

void FluidSystem::getPointsForScreen(vector<vec4>& particles, vector<vec1>& densities, vector<uvec1>& indices) {
    //std::cout << "getPointsForScreen." << std::endl;
    particles.clear();
    densities.clear();
    indices.clear();
    for (Cell& c : grid) {
        particles.push_back( toScreen(Grid::iToPos(c.i) ));
        densities.push_back(vec1(c.den));
        indices.push_back(uvec1(c.i));
    }
}

vec4 FluidSystem::toScreen(const vec2& point) {

    float ndcX = ((2.0f * point.x) / float(Grid::N+2)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(Grid::N+2)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0, 1.0);
}

void FluidSystem::printAll(){
    Accessor temp = Cell::printVar;
    Cell::printVar = density;
    std::cout << "Printing all values on both grids:\n";
    std::cout << "Current Density \"X\"\n" << grid << '\n';
    std::cout << "Old Density \"X0\"\n" << oldGrid << "\n\n";
    Cell::printVar = velocityX;
    std::cout << "Current vX \"U\"\n" << grid << '\n';
    std::cout << "Old vX \"U0\"\n" << oldGrid << "\n\n";
    Cell::printVar = velocityY;
    std::cout << "Current vY \"V\"\n" << grid << '\n';
    std::cout << "Old vY \"V0\"\n" << oldGrid << '\n' << std::endl;
    Cell::printVar = temp;
}

void FluidSystem::printCurrent(){
    Accessor temp = Cell::printVar;
    Cell::printVar = density;
    std::cout << "Printing Current Grid:\n";
    std::cout << "Current Density \"X\"\n" << grid << '\n';
    Cell::printVar = velocityX;
    std::cout << "Current vX \"U\"\n" << grid << '\n';
    Cell::printVar = velocityY;
    std::cout << "Current vY \"V\"\n" << grid << '\n';
    Cell::printVar = temp;
}

void FluidSystem::printOld(){
    Accessor temp = Cell::printVar;
    Cell::printVar = density;
    std::cout << "Printing Old Grid:\n";
    std::cout << "Old Density \"X0\"\n" << oldGrid << '\n';
    Cell::printVar = velocityX;
    std::cout << "Old vX \"U0\"\n" << oldGrid << '\n';
    Cell::printVar = velocityY;
    std::cout << "Old vY \"V0\"\n" << oldGrid << '\n';
    Cell::printVar = temp;
}

void FluidSystem::print(Accessor var, Grid& src){
    Accessor temp = Cell::printVar;
    Cell::printVar = var;
    std::cout << src << std::endl;
    Cell::printVar = temp;
}

void FluidSystem::test(){
    std::cout << "*************TESTING*************" << std::endl;
    printAll();
    std::cout << "******************" << std::endl;

    std::cout << "*************END TESTING*************" << std::endl;
}

static int stepCount = 0;

void FluidSystem::step() {

    // Step Velocity
    add( velocityX, velocityX, grid, oldGrid );
    add( velocityY, velocityY, grid, oldGrid );

    swap( velocityX, velocityX, oldGrid, grid );
    diffuse( velocityX, velocityX, viscosity, 1 );
    swap( velocityY, velocityY, oldGrid, grid);
    diffuse( velocityY, velocityY, viscosity, 2 );

    project();
    swap( velocityX, velocityX, oldGrid, grid );
    swap( velocityY, velocityY, oldGrid, grid );

    advect( velocityX, oldGrid, 1 );
    advect( velocityY, oldGrid, 2 );

    project();

    // Step Density
    add( density, density, grid, oldGrid );
    swap( density, density, oldGrid, grid );

    diffuse( density, density, diffusion, 0 );
    swap( density, density, oldGrid, grid );

    advect( density, grid, 0  );
    stepCount++;
    oldGrid.clear();
}

void FluidSystem::setup() {
    // give things an intial density
    for (int r = 1; r < grid.N + 1; ++r ) {
        for (int c = 1; c < grid.N + 1; ++c ) {
            Cell& currentCell = grid.at(r, c);
            // FIXME These are not good initial values for a real one. Just for testing.
            currentCell.den = r + c;
        }
    }
    fixBoundary(density,grid,0);
    fixBoundary(velocityX,grid,1);
    fixBoundary(velocityY,grid,2);
    fixBoundary(density,oldGrid,0);
    fixBoundary(velocityX,oldGrid,1);
    fixBoundary(velocityY,oldGrid,2);
    getPointsForScreen(particles, densities, indices);
}

// ***********
// Print Vec2
// ***********

std::ostream& operator<<( std::ostream& os, const vec2& v ){
    std::ostringstream ss;
    ss << std::setfill(' ') << '(' << std::setw(4);
    ss << v.x << ", " << std::setw(4) << v.y << ')';
    return os << ss.str();
}

#endif
