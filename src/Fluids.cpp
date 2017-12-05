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

// *****
// Cell
// *****

ivec2 Cell::coord() const { return Grid::iToCo(i); }
vec2 Cell::pos() const { return Grid::iToPos(i); }

// *****
// Grid
// *****

// Set Grid::printSpacing (int)     and     Cell::printVar (Accessor)   to change output.
std::ostream& operator<<( std::ostream& os, Grid& g ){
    std::ostringstream ss;
    for( int r=0; r<g.N+2; ++r ){
        for( int c=0; c<g.N+2; ++c ){
            if(c==0||c==g.N+1)
                ss << std::setfill('.');
            else if(c==1 && r != 0 && r != g.N+1)
                ss << std::setfill(' ');
            ss << std::setw( Grid::printSpacing );
            ss << std::to_string( Cell::printVar( g.at(r,c) ));
        }
        ss << '\n';
        if( r == g.N+1 )
            continue;
        for( int n=2; n < Grid::printSpacing; ++n )
            ss << std::setw( Grid::printSpacing ) << "" << std::setfill(' ') << std::setw( Grid::printSpacing * g.N )
                << "" << std::setfill('.') <<  std::setw( Grid::printSpacing + 1 ) << '\n';
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
    float t;
    for(int r=0; r < Grid::N+2; ++r)
        for(int c=0; c < Grid::N+2; ++c){
            t = varA(srcA.at(r,c));
            varB(srcB.at(r,t)) = varA(srcA.at(r,c));
            varA(srcA.at(r,t)) = t;
        }
}

void linearSolver( Accessor varA, Access varB, Grid& srcA, Grid& srcB, float scalarNumerator, float scalarDenominator ){
   for( int k=0; k<20; ++c ){
        for( int r=1; r < Grid::N+1; ++r ){
            for( int c=1; c < Grid::N+1; ++c ){
                varA(srcA.at(r,c)) = ( varB(srcB.at(r,c)) + scalarNumerator * ( varA(srcA.at(r-1,c)) + 
                            varA(srcA.at(r+1,c)) + varA(srcA.at(r,c-1)) + varA(srcA.at(r,c+1)) )) / scalarDenominator;
            }}
        fixBoundary( varA, srcA );}
}

void FluidSystem::diffuse( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB ){
    float scalarNumerator = dt * diffusion * Grid::N * Grid::N;
    linearSolver( varA, varB, srcA, srcB, scalarNumberator, (1.0f + 4.0f * scalarNumerator) );
}

void FluidSystem::advect( Accessor varA, Accessor varB, Grid& srcA, Grid& srcB ){
    float dt0 = dt * Grid::N;
    for( int r=1; r < Grid::N+1; ++r ){
        for( int c=1; c < Grid::N+1; ++c ){
            Cell& currentCell = srcA.at(r,c);
            vec2 backStep = currentCell.centerPosition() - vec2( dt * currentCell.vx, dt * currentCell.vy );
            
            // check bounds
            float upper = Grid::N + 0.5f;
            if( backStep.x < 0.5f ) backStep[0] = 0.5f;
            if( backStep.y < 0.5f ) backStep[1] = 0.5f;
            if( backStep.x >= upper ) backStep[0] = upper;
            if( backStep.y >= upper ) backStep[1] = uppper;
            
            ivec2 backCoord = grid.positionToCoordinates( backstep );     
            int br = backCords[0];
            int bc = backCoord[1];    
            float weightWest = float(backCoord.x) - bc;
            float weightEast = 1.0f - weightWest;
            float weightNorth = float(backCoord.y) - br;
            float weightSouth = 1.0f - weightNorth;
            
            varA(srcA.at(r,c)) = weightEast * ( weightSouth * varB(srcB(br, bc)) + weightNorth * varB(srcB(br, bc+1)) )
                              + weightWest * ( weightSouth * varB(srcB(br+1, bc)) + weightNorth * varB(srcB(br+1, bc+1)));
        }
    }
    fixBoundary( varA, srcA );
}

void FluidSystem::project(){
    for (int r = 1; r < grid.N + 1; ++r) {
        for (int c = 1; c < grid.N + 1; ++c) {
            // compute the gradient between each neighbors pressure
            Cell& currentCell = grid.at(r, c);
            currentCell.div = -0.5f * Grid::N * (grid.at(r+1,c).vx - grid.at(r-1).vx + grid.at(r,c+1).vy - grid.at(r,c-1).vy) / Grid::N;
            currentCell.p = 0.0f;
        }
    fixBoundary( divergence );
    fixBoundary( pressure );
    linearSolver( pressure, )
    }
}

template<typename T>
void FluidSystem::fixBoundary( FluidSystem::flag f ){
    std::function<vec2&(Cell&)> accessV;
    std::function<float&(Cell&)> accessF;
    switch( f ){
        case velocity : accessV =     ( &Cell::velocity );break;
        case density : accessF =      ( &Cell::density );break;
        case divergence : accessF =   ( &Cell::divergence );break;
        case pressure : accessF =     ( &Cell::pressure );break;
    }

    std::function<T&(Cell&)> accessor;
    if( f == velocity )
        accessor = *reinterpret_cast<std::function<T&(Cell&)>*>( &accessV );
    else
        accessor = *reinterpret_cast<std::function<T&(Cell&)>*>( &accessF );

    for( int i=1; i<=grid.N; ++i){
       accessor( grid.at( 0, i )) = accessor( grid.at( 1, i ));
       accessor( grid.at( grid.N+1, i )) = accessor( grid.at( grid.N, i ));
       accessor( grid.at( i, 0 )) = accessor( grid.at( i, 1 ));
       accessor( grid.at( i, grid.N+1 )) = accessor( grid.at( i, grid.N ));
       if( f == velocity ){
           accessor( grid.at( 0, i )).x     *= -1.0f;
           accessor( grid.at( grid.N+1, i )).x   *= -1.0f;
           accessor( grid.at( i, 0 )).y     *= 1.0f;
           accessor( grid.at( i, grid.N+1 )).y   *= 1.0f;
       }
    }
   accessor( grid.at( 0, 0 )) = 0.5f * ( accessor( grid.at( 1, 0 )) + accessor( grid.at( 0, 1 )));
   accessor( grid.at( 0, grid.N+1 )) = 0.5f * ( accessor( grid.at( 1, grid.N+1 )) + accessor( grid.at( 0, grid.N )));
   accessor( grid.at( grid.N+1, 0 )) = 0.5f * ( accessor( grid.at( grid.N, 0 )) + accessor( grid.at(grid.N+1, 1 )));
   accessor( grid.at( grid.N+1, grid.N+1 )) = 0.5f * ( accessor( grid.at( grid.N, grid.N+1 )) + accessor( grid.at( grid.N+1, grid.N )));
}

// drawing stuff

FluidSystem::FluidSystem(int grid_size = 50, int dx_ = 10, int dy_ = 10, float time_step = (1.0f/60f), ) : grid(Grid(grid_size, dx, dy)), oldGrid(Grid(grid_size, dx, dy)), dt(time_step), diffusion(diff_), viscosity(vics_), fluid_pass(-1, fluid_pass_input, {fluid_vertex_shader, fluid_geometry_shader, particle_fragment_shader}, {/*uniforms*/}, {"fragment_color"}){
                    getPointsForScreen(particles, densities, indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    fluid_pass_input.assign(1, "density", densities.data(), densities.size(), 1, GL_FLOAT);
                }

void FluidSystem::prepareDraw() {
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
    getPointsForScreen(particles, densities, indices);
    fluid_pass.updateVBO(0, particles.data(), particles.size());
    fluid_pass.updateVBO(1, densities.data(), densities.size());
    fluid_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0));
}

void FluidSystem::getPointsForScreen(vector<vec4>& particles, vector<vec1>& densities, vector<uvec1>& indices) {
    particles.clear();
    densities.clear();
    indices.clear();
    int count = 0;
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& c = grid.at(i, j);
            vec4 particle = toScreen(c.particle);
            particles.push_back(particle);
            densities.push_back(vec1(c.density));
            indices.push_back(uvec1(count));
            ++count;
        }
    }
}

vec4 FluidSystem::toScreen(const vec2& point) {
    float ndcX = ((2.0f * point.x) / float(width * 2)) - 1.0f;
    float ndcY = ((2.0f * point.y) / float(height * 2)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0, 1.0);
}

void FluidSystem::step() {
    //copy the current array into the old array
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    // get force input from ui
    float ui_input_density;
    vec2 ui_input_velocity;
    std::cout << "STEPPING\nGrid : " << std::endl;
    std::cout << grid << std::endl;
    //std::cout << "OldGrid : " << std::endl;
    //std::cout << oldGrid << std::endl;

    // Error is in diffuseVelocity
    // **************************
    diffuseVelocity();
    // **************************
    std::cout << "Done Diffusing\nGrid : " << std::endl;
    std::cout << grid << std::endl;

    project();
    std::copy(grid.begin(), grid.end(), oldGrid.begin());
    advectVelocity();
    project();
    diffuseDensity();
    advectDensity();
}

void FluidSystem::setup() {
    // give things an intial density
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& c = grid.at(i, j);
            // give intial parameters
            c.density = 10.0f;
            c.velocity = vec2(0.0f, 0.001f);
        }
    }
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
