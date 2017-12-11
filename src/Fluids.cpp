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
#include <GLFW/glfw3.h>
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
    float dt0 = dt * float(Grid::N);
    for( int r=1; r < Grid::N+1; ++r ){
        for( int c=1; c < Grid::N+1; ++c ){
            float x = float(r) - dt0 * src.at(r,c).vx; 
            float y = float(c) - dt0 * src.at(r,c).vy;
            // check bounds
            float upper = float(Grid::N) + 0.5f;
            if( x < 0.5f ) x = 0.5f;
            if( y < 0.5f ) y = 0.5f;
            if( x > upper ) x = upper;
            if( y > upper ) y = upper;
            
            int br = int(x);
            int bc = int(y);    
            float weightWest = x - float(br);
            float weightEast = 1.0f - weightWest;
            float weightNorth = y - float(bc);
            float weightSouth = 1.0f - weightNorth;
            
            var(grid.at(r,c)) = weightEast * ( weightSouth * var(oldGrid.at(br, bc)) + weightNorth * var(oldGrid.at(br, bc+1)) ) + weightWest * ( weightSouth * var(oldGrid.at(br+1, bc)) + weightNorth * var(oldGrid.at(br+1, bc+1)));
        }}
    fixBoundary( var, grid, f );
}

void FluidSystem::project(){
    //std::cout << "Project." << std::endl;
    float h = 1.0f/float(Grid::N);
    for (int r=1; r < Grid::N+1; ++r) {
        for (int c=1; c < Grid::N+1; ++c) {
            // compute the gradient between each neighbors pressure
            // NOTE: I am using .vx and .vy in oldCell as a cache for pressure data
            // we no longer need those old positions at this point and this will allow us to use 2 fewer floats for each Cell.
            oldGrid.at(r,c).vy = -0.5f * h * (grid.at(r+1,c).vx - grid.at(r-1,c).vx + grid.at(r,c+1).vy - grid.at(r,c-1).vy);
            oldGrid.at(r,c).vx = 0.0f;
        }}
    fixBoundary( divergence, oldGrid, 0 );
    fixBoundary( pressure, oldGrid, 0 );
    // dont forget we are temporarily occupying oldGrid.vx & .vy here
    linearSolver( pressure, divergence, oldGrid, oldGrid, 1.0f, 4.0f, 0 );
    for( int r=1; r < Grid::N+1; ++r ){
        for( int c=1; c < Grid::N+1; ++c ){
            grid.at(r,c).vx -= -0.5f * ( oldGrid.at(r+1,c).vx - oldGrid.at(r-1,c).vx )/h;
            grid.at(r,c).vy -= -0.5f * ( oldGrid.at(r,c+1).vx - oldGrid.at(r,c-1).vx )/h;
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

FluidSystem::FluidSystem(int grid_size, int dx_, int dy_, float time_step, float diff_, float visc_ ) : grid(Grid(grid_size, dx_, dy_)), oldGrid(Grid(grid_size, dx_, dy_)), dt(time_step), diffusion(diff_), viscosity(visc_), fluid_pass(-1, fluid_pass_input, {grid_vertex_shader, grid_geometry_shader, grid_fragment_shader}, {/*uniforms*/}, {"fragment_color"}), velocity_pass(-1, velocity_pass_input, {velocity_vertex_shader, velocity_geometry_shader, velocity_fragment_shader}, {/*uniforms*/}, {"fragment_color"}){
                    getPointsForScreen(particles, velocities, indices, vel_indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    velocity_pass_input.assign(0, "velocity", velocities.data(), velocities.size(), 2, GL_FLOAT);
                }

void FluidSystem::prepareDraw() {
    //std::cout << "Prepare Draw." << std::endl;
    fluid_pass_input.assign_index(indices.data(), indices.size(), 3);
    fluid_pass = RenderPass(-1,
                               fluid_pass_input,
                               {
                                       grid_vertex_shader,
                                       grid_geometry_shader,
                                       grid_fragment_shader
                               },
                               { /* uniforms */ },
                               { "fragment_color" }
    );

    velocity_pass_input.assign_index(vel_indices.data(), vel_indices.size(), 2);
    velocity_pass = RenderPass(-1,
                                velocity_pass_input,
                                {
                                        velocity_vertex_shader,
                                        velocity_geometry_shader,
                                        velocity_fragment_shader
                                },
                                { /* uniforms */},
                                { "fragment_color" }
    );
}

void FluidSystem::draw() {
    //std::cout << "draw." << std::endl;
    getPointsForScreen(particles, velocities, indices, vel_indices);
    fluid_pass.updateVBO(0, particles.data(), particles.size());
    fluid_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, indices.size()* 3, GL_UNSIGNED_INT, 0));
    velocity_pass.updateVBO(0, velocities.data(), velocities.size());
    velocity_pass.setup();
    CHECK_GL_ERROR(glDrawElements( GL_LINES, vel_indices.size()*2, GL_UNSIGNED_INT, 0));
}

void FluidSystem::getPointsForScreen(vector<vec4>& particles, vector<vec4>& velocities, vector<uvec3>& indices, vector<uvec2>& vel_indices ) {
    //std::cout << "getPointsForScreen." << std::endl;
    particles.clear();
    indices.clear();
    int i = 0;
    int j =0;
    for (int r=1;r<Grid::N+1;++r) {
        for(int c=1;c<Grid::N+1;++c){
            // Density
            particles.push_back( toScreen( vec3( float(r-1), float(c-1), grid.at( r, c ).den )));
            particles.push_back( toScreen( vec3( float(r-1), float(c), grid.at( r, c+1 ).den )));
            particles.push_back( toScreen( vec3( float(r), float(c-1), grid.at( r+1, c ).den )));
            particles.push_back( toScreen( vec3( float(r), float(c), grid.at( r+1, c+1 ).den )));
            indices.push_back( uvec3( i, i+1, i+3 ));
            indices.push_back( uvec3( i, i+3, i+2 ));
            i+=4;

            // Velocity
            vec2 center = Grid::coToPos(r-1,c-1);
            velocities.push_back( toScreen( center ));
            velocities.push_back( toScreen( center + vec2( grid.at(r,c).vx, grid.at(r,c).vy )));
            vel_indices.push_back( uvec2( j, j+1 ));
            j+=2;
        }
    }
}

vec4 FluidSystem::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.y) / float(Grid::N)) - 1.0f;
    float ndcY = ((2.0f * point.x) / float(Grid::N)) - 1.0f;
    return vec4(ndcX, ndcY, point.z, 1.0f);
}

vec4 FluidSystem::toScreen(const vec2& pos){
    float ndcX = ((2.0f * pos.y) / float(Grid::N)) - 1.0f;
    float ndcY = ((2.0f * pos.x) / float(Grid::N)) - 1.0f;
    return vec4(ndcX, ndcY, 1.0f, 1.0f);
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
    // For Unit Testing. 
}

void FluidSystem::keyWasPressed(int keyCode){

}

void FluidSystem::mouseDragged(float x, float y){
    mouse0 = mouse;
    mouse = vec3( (x / float(width)) * float(Grid::N) , (y / float(height)) * float(Grid::N), 1.0f);
    if( glm::distance(mouse,mouse0) > 5.0f )
        mouse0 = mouse;
}

static int stepCount = 0;
void FluidSystem::step() {

    if( isDragging && mouse_button == GLFW_MOUSE_BUTTON_LEFT && mouse != mouse0 ) {
        //std::cout << "Screen : " << mouse.x << ", " << mouse.y  << "\t";
        vec2 dir = glm::normalize(vec2(mouse - mouse0));
        int r = mouse[1];
        int c = mouse[0];
        //std::cout << "RC : " << r << ", " << c << "\t";
        //std::cout << "vx += " << (dir.y * 10.0f * float(Grid::N)) << "\tvy += " << (dir.x * 10.0f * float(Grid::N)) << std::endl;
        oldGrid.at(r,c).vx += dir.y * 10.0f * float(Grid::N);
        oldGrid.at(r,c).vy += dir.x * 10.0f * float(Grid::N);
    } else if( isDragging && mouse_button == GLFW_MOUSE_BUTTON_RIGHT){
        oldGrid.at(mouse[1],mouse[0]).den += 10.0f * float(Grid::N);
    }

    // Play with values of oldGrid here for "input"
    oldGrid.at(5,5).den += 10.0f;
    oldGrid.at(25,25).den += 25.0f;

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
    fixBoundary(density,grid,0);
    fixBoundary(velocityX,grid,1);
    fixBoundary(velocityY,grid,2);
    fixBoundary(density,oldGrid,0);
    fixBoundary(velocityX,oldGrid,1);
    fixBoundary(velocityY,oldGrid,2);
    getPointsForScreen(particles, velocities, indices, vel_indices);
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
