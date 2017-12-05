//
// Created by Joshua Cristol on 12/2/17.
//

#ifndef FLUIDS_CPP
#define FLUIDS_CPP

#include <glm/gtx/string_cast.hpp>
#include "Fluids.h"
#include <functional>
#include "Shaders.h"
using std::function;

// ***********
// Print Vec2
// ***********

std::ostream& operator<<( std::ostream& os, const vec2& v ){
    std::ostringstream ss;
    ss << std::setfill(' ') << '(' << std::setw(4);
    ss << v.x << ", " << std::setw(4) << v.y << ')';
    return os << ss.str();
}

// *****
// Grid
// *****

// Set Grid::printSpacing (int)     and     Cell::printVar (Accessor)   to change output.
std::ostream& operator<<( std::ostream& os, const Grid& g ){
    std::ostringstream ss;
    for( int r=0; r<g.N+2; ++r ){
        for( int c=0; c<g.N+2; ++c ){
            if(c==0||c==g.N+1)
                ss << std::setfill('.');
            else if(c==1 && r != 0 && r != g.N+1)
                ss << std::setfill(' ');
            ss << std::setw( printSpacing );
            ss << Cell::printVar( grid.at(r,c) );
        }
        ss << '\n';
        if( r == g.N+1 )
            continue;
        for( int n=2; n<printSpacing; ++n )
            ss << std::setw( printSpacing ) << "" << std::setfill(' ') << std::setw( printSpacing * g.N ) 
                << "" << std::setfill('.') <<  std::setw( printSpacing+1 ) << '\n';
    }
    return os << ss.str();
}

// ************
// FluidSystem
// ************

void FluidSystem::advectVelocity(){
    std::cout << "Advect Velocity\n";
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& currentCell = grid.at(i, j);
            vec2 particleCell = grid.cellToParticle(i, j);
            vec2& vel = currentCell.velocity;
            vec2 backwardStepParticle = particleCell - ( dt * currentCell.velocity );
            Cell iCell = interpolate(backwardStepParticle);
            currentCell.velocity = iCell.velocity;
        }
    }
}

void FluidSystem::advectDensity(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            Cell& currentCell = grid.at(i, j);
            vec2 particleCell = grid.cellToParticle(i, j);
            vec2 backwardStepParticle = particleCell - dt * currentCell.velocity;
            Cell iCell = interpolate(backwardStepParticle);
            currentCell.density= iCell.density;
        }
    }
}

void FluidSystem::diffuseVelocity(){
    std::stringstream ss;
    ss << "Diffusing Velocity\n";
    float viscoity = grid.N * grid.N * 0.2;
    for (int k = 0; k < 20; ++k) {
        ss << "K : " << k << '\n';
        for (int i = 1; i < grid.N + 1; ++i) {
            for (int j = 1; j < grid.N + 1; ++j) {
                ss << '\t' << '(' << i << ", " << j << ") :\t";
                Cell& currentCell = grid.at(i, j);
                Cell& north = oldGrid.at(i, j - 1);
                Cell& south = oldGrid.at(i, j + 1);
                Cell& east = oldGrid.at(i + 1, j);
                Cell& west = oldGrid.at(i - 1, j);
                //make left hand just horizonatal component same for the right hand
                vec2 leftHandLaplace = ( 1.0f / (grid.dx * grid.dx)) * (east.velocity - (2.0f * currentCell.velocity) + west.velocity);
                vec2 rightHandLaplace = ( 1.0f / (grid.dy * grid.dy)) * (south.velocity - (2.0f * currentCell.velocity) + north.velocity);
                vec2 laplace = viscoity * (leftHandLaplace + rightHandLaplace);
                ss << "Velocity : " << currentCell.velocity;
                ss << "\tLaplace : " << laplace << std::endl;
                currentCell.velocity = dt * laplace;
            }
        }
        fixBoundary<vec2>(FluidSystem::velocity);
    }
    //std::cout << ss.str();
}

void FluidSystem::diffuseDensity(){
    float viscoity = grid.N * grid.N * 0.2;
    for (int k = 0; k < 20; ++k) {
        for (int i = 1; i < grid.N + 1; ++i) {
            for (int j = 1; j < grid.N + 1; ++j) {
                Cell& currentCell = grid.at(i, j);
                Cell& north = oldGrid.at(i, j - 1);
                Cell& south = oldGrid.at(i, j + 1);
                Cell& east = oldGrid.at(i + 1, j);
                Cell& west = oldGrid.at(i - 1, j);
                //make left hand just horizonatal component same for the right hand
                float leftHandLaplace = (east.density - (2.0f * currentCell.density) + west.density) / (grid.dx * grid.dx);
                float rightHandLaplace = (south.density - (2.0f * currentCell.density) + north.density) / (grid.dy * grid.dy);
                float laplace = (leftHandLaplace + rightHandLaplace) * viscoity;
                currentCell.density = laplace;
                currentCell.pressure = currentCell.density / (grid.dx * grid.dy);
            }
        }
    }
}

void FluidSystem::project(){
    for (int i = 1; i < grid.N + 1; ++i) {
        for (int j = 1; j < grid.N + 1; ++j) {
            // compute the gradient between each neighbors pressure
            Cell& currentCell = grid.at(i, j);
            Cell& north = oldGrid.at(i, j - 1);
            Cell& south = oldGrid.at(i, j + 1);
            Cell& east = oldGrid.at(i + 1, j);
            Cell& west = oldGrid.at(i - 1, j);
            vec2 southGrad = vec2(0.0f, currentCell.pressure - south.pressure);
            vec2 northGrad = vec2(0.0f, currentCell.pressure - north.pressure);
            vec2 eastGrad = vec2(currentCell.pressure - east.pressure, 0.0f);
            vec2 westGrad = vec2(currentCell.pressure - west.pressure, 0.0f);
            vec2 avgGrad = (southGrad + northGrad + eastGrad + westGrad) / 4.0f;
            currentCell.velocity = currentCell.velocity - avgGrad;
        }

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

FluidSystem::FluidSystem(int grid_size, int dx, int dy, float time_step) : grid(Grid(grid_size, dx, dy)), oldGrid(Grid(grid_size, dx, dy)), dt(time_step), fluid_pass(-1, fluid_pass_input, {fluid_vertex_shader, fluid_geometry_shader, particle_fragment_shader}, {/*uniforms*/}, {"fragment_color"}){
                    getPointsForScreen(particles, densities, indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    fluid_pass_input.assign(1, "density", densities.data(), densities.size(), 1, GL_FLOAT);
                    std::cout << "\n\nGrid : \n" << grid << "\n\n" << std::flush;
                    std::cout << "OldGrid : \n" << oldGrid << "\n\n" << std::flush;
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
#endif
