//
// JoShuA CrIsToL wAs HiGh OcTaNe On 12/2/17.
//

#ifndef FLUIDS_CPP
#define FLUIDS_CPP

#include <glm/gtx/string_cast.hpp>
#include "Fluids.h"
#include <string>
#include "Shaders.h"
#include <GLFW/glfw3.h>

// ************
// FluidSystem
// ************

void FluidSystem::add( float*& x, float*& s ){
    //std::cout << "Add." << std::endl;
    for(int r=0; r < grid.N+2; ++r )
        for(int c=0; c < grid.N+2; ++c)
            grid.at(x,r,c) += dt * grid.at(s,r,c);
}

void FluidSystem::linearSolver( float*& x, float*& x0, float scalarNumerator, float scalarDenominator, short f ){
    //std::cout << "Linear Solver." << std::endl;
   for( int k=0; k<20; ++k ){
        for( int r=1; r < grid.N+1; ++r ){
            for( int c=1; c < grid.N+1; ++c ){
                grid.at(x,r,c) = ( grid.at(x0,r,c) + scalarNumerator * ( grid.at(x,r-1,c) + grid.at(x,r+1,c) + grid.at(x,r,c-1) + grid.at(x,r,c+1) )) / scalarDenominator;
            }}
        fixBoundary( x, f );}
}

void FluidSystem::diffuse( float*& x, float*& x0,  float scalar, short f ){
    //std::cout << "Diffuse." << std::endl;
    float scalarNumerator = dt * scalar * grid.N * grid.N;
    linearSolver( x, x0, scalarNumerator, (1.0f + 4.0f * scalarNumerator), f );
}

// FIXME make an Advect Colors to optimize the swap of all of our color values at once. 
void FluidSystem::advect( float*& d, float*& d0, float*& u, float*& v, short f ){
    //std::cout << "Advect." << std::endl;
    float dt0 = dt * float(grid.N);
    for( int r=1; r < grid.N+1; ++r ){
        for( int c=1; c < grid.N+1; ++c ){
            float x = float(r) - dt0 * grid.at(u,r,c); 
            float y = float(c) - dt0 * grid.at(v,r,c);
            // check bounds
            float upper = float(grid.N) + 0.5f;
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
            
            grid.at(d,r,c) = weightEast * ( weightSouth * grid.at(d0,br, bc) + weightNorth * grid.at(d0,br, bc+1) ) + weightWest * ( weightSouth * grid.at(d0,br+1, bc) + weightNorth * grid.at(d0, br+1, bc+1));
        }}
    fixBoundary( d, f );
}

// We can basically add any other values here as well.
void FluidSystem::advectRGB( float*& dR, float*& dR0, float*& dG, float*& dG0, float*& dB, float*& dB0, float*& u, float*& v, short f ){
    //std::cout << "AdvectRGB." << std::endl;
    float dt0 = dt * float(grid.N);
    for( int r=1; r < grid.N+1; ++r ){
        for( int c=1; c < grid.N+1; ++c ){
            float x = float(r) - dt0 * grid.at(u,r,c); 
            float y = float(c) - dt0 * grid.at(v,r,c);
            // check bounds
            float upper = float(grid.N) + 0.5f;
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
            
            grid.at(dR,r,c) = weightEast * ( weightSouth * grid.at(dR0,br, bc) + weightNorth * grid.at(dR0,br, bc+1) ) + weightWest * ( weightSouth * grid.at(dR0,br+1, bc) + weightNorth * grid.at(dR0, br+1, bc+1));
            grid.at(dG,r,c) = weightEast * ( weightSouth * grid.at(dG0,br, bc) + weightNorth * grid.at(dG0,br, bc+1) ) + weightWest * ( weightSouth * grid.at(dG0,br+1, bc) + weightNorth * grid.at(dG0, br+1, bc+1));
            grid.at(dB,r,c) = weightEast * ( weightSouth * grid.at(dB0,br, bc) + weightNorth * grid.at(dB0,br, bc+1) ) + weightWest * ( weightSouth * grid.at(dB0,br+1, bc) + weightNorth * grid.at(dB0, br+1, bc+1));
        }}
    fixBoundary( dR, f );
    fixBoundary( dG, f );
    fixBoundary( dB, f );
}

void FluidSystem::project( float*& u, float*& v, float*& p, float*& div ){
    //std::cout << "Project." << std::endl;
    float h = 1.0f/float(grid.N);
    for (int r=1; r < grid.N+1; ++r) {
        for (int c=1; c < grid.N+1; ++c) {
            // compute the gradient between each neighbors pressure
            // NOTE: I am using .vx and .vy in oldCell as a cache for pressure data
            // we no longer need those old positions at this point and this will allow us to use 2 fewer floats for each Cell.
            grid.at(div,r,c) = -0.5f * h * (grid.at(u,r+1,c) - grid.at(u,r-1,c) + grid.at(v,r,c+1) - grid.at(v,r,c-1));
            grid.at(p,r,c) = 0.0f;
        }}
    fixBoundary( div, 0 );
    fixBoundary( p, 0 );
    // dont forget we are temporarily occupying oldGrid.vx & .vy here
    linearSolver( p, div, 1.0f, 4.0f, 0 );
    for( int r=1; r < grid.N+1; ++r ){
        for( int c=1; c < grid.N+1; ++c ){
            grid.at(u,r,c) -= -0.5f * ( grid.at(p,r+1,c) - grid.at(p,r-1,c) )/h;
            grid.at(v,r,c) -= -0.5f * ( grid.at(p,r,c+1) - grid.at(p,r,c-1) )/h;
        }}
    fixBoundary( u, 1 );
    fixBoundary( v, 2 );
}

void FluidSystem::fixBoundary( float*& x, short f ){
    //std::cout << "Fix Boundary." << std::endl;
    for( int i=1; i<=grid.N; ++i){
       grid.at( x, 0, i ) =         (f==1? -1.0f : 1.0f) * grid.at( x, 1, i );
       grid.at( x, grid.N+1, i ) =  (f==1? -1.0f : 1.0f) * grid.at( x, grid.N, i );
       grid.at( x, i, 0 ) =         (f==2? -1.0f : 1.0f) * grid.at( x, i, 1 );
       grid.at( x, i, grid.N+1 ) =  (f==2? -1.0f : 1.0f) * grid.at( x, i, grid.N );
    }
   grid.at( x, 0, 0 ) = 0.5f * ( grid.at( x, 1, 0 ) + grid.at( x, 0, 1 ));
   grid.at( x, 0, grid.N+1 ) = 0.5f * ( grid.at( x, 1, grid.N+1 ) + grid.at( x, 0, grid.N ));
   grid.at( x, grid.N+1, 0 ) = 0.5f * ( grid.at( x, grid.N, 0 ) + grid.at( x, grid.N+1, 1 ));
   grid.at( x, grid.N+1, grid.N+1 ) = 0.5f * ( grid.at( x, grid.N, grid.N+1 ) + grid.at( x, grid.N+1, grid.N ));
}

// drawing stuff

FluidSystem::FluidSystem(int grid_size, float time_step, float diff_, float visc_ ) : 
    grid(Grid(grid_size)), dt(time_step), diffusion(diff_), viscosity(visc_), 
    fluid_pass(-1, fluid_pass_input, {grid_vertex_shader, grid_geometry_shader, grid_fragment_shader}, {/*uniforms*/}, {"fragment_color"}), 
    velocity_pass(-1, velocity_pass_input, {velocity_vertex_shader, velocity_geometry_shader, velocity_fragment_shader}, {/*uniforms*/}, {"fragment_color"}), activeColor(grid.densR_P)
                {
                    getPointsForScreen(particles, color, velocities, indices, vel_indices);
                    fluid_pass_input.assign(0, "vertex_position", particles.data(), particles.size(), 4, GL_FLOAT);
                    fluid_pass_input.assign(1, "vertex_color", color.data(), color.size(), 3, GL_FLOAT);
                    velocity_pass_input.assign(0, "velocity", velocities.data(), velocities.size(), 4, GL_FLOAT);
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
    getPointsForScreen(particles, color, velocities, indices, vel_indices);
    velocity_pass.updateVBO(0, velocities.data(), velocities.size());
    velocity_pass.setup();
    CHECK_GL_ERROR(glDrawElements( GL_LINES, vel_indices.size()*2, GL_UNSIGNED_INT, 0));
    
    fluid_pass.updateVBO(0, particles.data(), particles.size());
    fluid_pass.updateVBO(1, color.data(), color.size());
    fluid_pass.setup();
    CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, indices.size()* 3, GL_UNSIGNED_INT, 0));
}

void FluidSystem::getPointsForScreen(vector<vec4>& particles, vector<vec3>& color, vector<vec4>& velocities, vector<uvec3>& indices, vector<uvec2>& vel_indices ) {
    //std::cout << "getPointsForScreen." << std::endl;
    particles.clear();
    color.clear();
    indices.clear();
    velocities.clear();
    vel_indices.clear();
    int i = 0;
    int j = 0;
    for ( int r=1; r < grid.N+1; ++r) {
        for( int c=1; c < grid.N+1; ++c){
            // Density
            particles.push_back( toScreen( vec3( float(r-1), float(c-1), grid.at( grid.densR, r, c ) + grid.at( grid.densG, r, c ) + grid.at( grid.densB, r, c ) )));
            color.push_back( vec3( grid.at( grid.densR, r, c ), grid.at( grid.densG, r, c ),  grid.at( grid.densB, r, c ) ));
            
            particles.push_back( toScreen( vec3( float(r-1), float(c), grid.at( grid.densR, r, c+1 ) + grid.at( grid.densG, r, c+1 ) + grid.at( grid.densB, r, c+1 ) )));
            color.push_back( vec3( grid.at( grid.densR, r, c+1 ), grid.at( grid.densG, r, c+1 ), grid.at( grid.densB, r, c+1 ) ));

            particles.push_back( toScreen( vec3( float(r), float(c-1), grid.at( grid.densR, r+1, c ) + grid.at( grid.densG, r+1, c ) + grid.at( grid.densB, r+1, c ) )));
            color.push_back( vec3( grid.at( grid.densR, r+1, c ), grid.at( grid.densG, r+1, c ), grid.at( grid.densB, r+1, c ) ));

            particles.push_back( toScreen( vec3( float(r), float(c), grid.at( grid.densR, r+1, c+1 ) + grid.at( grid.densG, r+1, c+1 ) + grid.at( grid.densB, r+1, c+1 ) )));
            color.push_back( vec3( grid.at( grid.densR, r+1, c+1 ), grid.at( grid.densG, r+1, c+1 ), grid.at( grid.densB, r+1, c+1 ) ));

            indices.push_back( uvec3( i, i+1, i+3 ));
            indices.push_back( uvec3( i, i+3, i+2 ));
            i+=4;

            // Velocity
            vec2 center = grid.coToPos(r - 1, c - 1);
            velocities.push_back( toScreen( center ));
            velocities.push_back( toScreen( center + vec2( grid.at( grid.velX, r, c ), grid.at( grid.velY, r, c ) )));
            vel_indices.push_back( uvec2( j, j+1 ));
            j+=2;
        }
    }
    
}

vec4 FluidSystem::toScreen(const vec3& point) {
    float ndcX = ((2.0f * point.y) / float(grid.N)) - 1.0f;
    float ndcY = ((2.0f * point.x) / float(grid.N)) - 1.0f;
    float d = point.z;
    if( d <= 0.0f )
        d = 0.00001f;
    return vec4(ndcX, ndcY, d, 1.0f);
}

vec4 FluidSystem::toScreen(const vec2& pos){
    float ndcX = ((2.0f * pos.y) / float(grid.N)) - 1.0f;
    float ndcY = ((2.0f * pos.x) / float(grid.N)) - 1.0f;
    return vec4(ndcX, ndcY, 0.0f, 1.0f);
}

void FluidSystem::keyWasPressed( int action, int key ){
    //std::cout << " Action : " << action << "\tKey : " << key << std::endl;
    
    float diffsens = 0.0001f;
    float viscsens = 0.01f;
    float forcesens = 1.0f;
    float amountsens = 1.0f;

    if( action == 1 && key == GLFW_KEY_LEFT_CONTROL){
        vacuum = ! vacuum;
        std::cout << "Vacuum " << ( vacuum ? "Activated" : "Deactivated") << std::endl;
    }else if( action == 1 && key == GLFW_KEY_R ){
        activeColor=grid.densR_P;
        std::cout << "Using Red" << std::endl;
    }else if( action == 1 && key == GLFW_KEY_G ){
        activeColor=grid.densG_P;
        std::cout << "Using Green" << std::endl;
    }else if( action == 1 && key == GLFW_KEY_B ){
        activeColor=grid.densB_P;
        std::cout << "Using Blue" << std::endl;
    }else if( action == 1 && key == GLFW_KEY_C ){
        grid.clear();
        cosas.clear();
        std::cout << "Screen Cleared" << std::endl;
    }else if( action == 1 && key == GLFW_KEY_BACKSLASH ){
        std::cout << "Current Settings - Diffusion : " << diffusion << "\tViscocity : " << viscosity << "\tForce : " << force << "\tAmount : " << amount << std::endl; 
    }else if( action == 1 && key == GLFW_KEY_LEFT_BRACKET){
        viscosity -= viscsens;
        if( viscosity < 0.0f )
            viscosity = 0.0f;
        std::cout << "Viscosity Reduced to : " << viscosity << std::endl;
    }else if( action == 1 && key == GLFW_KEY_RIGHT_BRACKET){
        viscosity += viscsens;
        if( viscosity > 100.0f )
            viscosity = 100.0f;
        std::cout << "Viscosity Increased to : " << viscosity << std::endl;
    }else if( action == 1 && key == GLFW_KEY_MINUS){
        diffusion -= diffsens;
        if( diffusion < 0.0f )
            diffusion = 0.0f;
        std::cout << "Diffusion Reduced to : " << diffusion << std::endl;
    }else if( action == 1 && key == GLFW_KEY_EQUAL){
        diffusion += diffsens;
        if( diffusion > 1.0f )
            diffusion = 1.0f;
        std::cout << "Diffusion Increased to : " << diffusion << std::endl;
    }else if( action == 1 && key == GLFW_KEY_SEMICOLON){
        force -= forcesens;
        if( force < 0.0f )
            force = 0.0f;
        std::cout << "Force Reduced to : " << force << std::endl;
    }else if( action == 1 && key == GLFW_KEY_APOSTROPHE){
        force += forcesens;
        if( force > 10000.0f )
            force = 10000.0f;
        std::cout << "Force Increased to : " << force << std::endl;
    }else if( action == 1 && key == GLFW_KEY_PERIOD){
        amount -= amountsens;
        if( amount < 0.0f )
            amount = 0.0f;
        std::cout << "Amount Reduced to : " << amount << std::endl;
    }else if( action == 1 && key == GLFW_KEY_SLASH){
        amount += amountsens;
        if( amount > 10000.0f )
            amount = 10000.0f;
        std::cout << "Amount Increased to : " << amount << std::endl;
    }else if( action == 1 && key == GLFW_KEY_1 ){
        cosas.push_back( Cosa( grid.coToI(mouse[1]+1, mouse[0])+1,  activeColor, ( vacuum ? -1.0f : 1.0f ) * amount * float(grid.N) ));
        std::cout << "Added Cosa " << cosas.size() << std::endl;
    }else if( action == 1 && key == GLFW_KEY_UP ){
        cosas.push_back( Cosa( grid.coToI(mouse[1]+1, mouse[0]+1), grid.velX_P, force ));
        std::cout << "Added Cosa " << cosas.size() << std::endl;
    }else if( action == 1 && key == GLFW_KEY_DOWN ){
        cosas.push_back( Cosa( grid.coToI(mouse[1]+1, mouse[0]+1), grid.velX_P, -force ));
        std::cout << "Added Cosa " << cosas.size() << std::endl;
    }else if( action == 1 && key == GLFW_KEY_RIGHT ){
        cosas.push_back( Cosa( grid.coToI(mouse[1]+1, mouse[0]+1), grid.velY_P, force ));
        std::cout << "Added Cosa " << cosas.size() << std::endl;
    }else if( action == 1 && key == GLFW_KEY_LEFT ){
        cosas.push_back( Cosa( grid.coToI(mouse[1]+1, mouse[0]+1), grid.velY_P, -force ));
        std::cout << "Added Cosa " << cosas.size() << std::endl;
    }



}

void FluidSystem::mouseDragged(float x, float y){
    mouse0 = mouse;
    mouse = vec3( (x / float(width)) * float(grid.N) , (y / float(height)) * float(grid.N), 1.0f);
    if( glm::distance(mouse,mouse0) > 5.0f )
        mouse0 = mouse;
}

static int stepCount = 0;
void FluidSystem::step() {

    // User Input
    if( isDragging && mouse_button == GLFW_MOUSE_BUTTON_LEFT && mouse != mouse0 ) {
        vec2 dir = vec2(mouse - mouse0); //glm::normalize(vec2(mouse - mouse0));
        int r = mouse[1]+1;
        int c = mouse[0]+1;
        grid.at( grid.velX_P, r, c ) += dir.y * force * float(grid.N);
        grid.at( grid.velY_P, r, c ) += dir.x * force * float(grid.N);
    } else if( isDragging && mouse_button == GLFW_MOUSE_BUTTON_RIGHT){
        float amt = ( vacuum ? -1.0f : 1.0f ) * amount * float(grid.N);
        grid.at(activeColor, mouse[1]+1, mouse[0]+1) += amt;
    }
    
    // Cosas
    for(Cosa& cosita : cosas)
        cosita.step();

    // Step Velocity
    add( grid.velX, grid.velX_P );
    add( grid.velY, grid.velY_P );

    Grid::swap( grid.velX, grid.velX_P );
    diffuse( grid.velX, grid.velX_P, viscosity, 1 );
    Grid::swap( grid.velY, grid.velY_P );
    diffuse( grid.velY, grid.velY_P, viscosity, 2 );

    project( grid.velX, grid.velY, grid.velX_P, grid.velY_P );
    Grid::swap( grid.velX, grid.velX_P );
    Grid::swap( grid.velY, grid.velY_P );

    advect( grid.velX, grid.velX_P, grid.velX_P, grid.velY_P, 1 );
    advect( grid.velY, grid.velY_P, grid.velX_P, grid.velY_P, 2 );

    project( grid.velX, grid.velY, grid.velX_P, grid.velY_P );

    // Step Density
    add( grid.densR, grid.densR_P );
    Grid::swap( grid.densR, grid.densR_P );
    add( grid.densG, grid.densG_P );
    Grid::swap( grid.densG, grid.densG_P );
    add( grid.densB, grid.densB_P );
    Grid::swap( grid.densB, grid.densB_P );

    diffuse( grid.densR, grid.densR_P, diffusion, 0 );
    Grid::swap( grid.densR, grid.densR_P );
    diffuse( grid.densG, grid.densG_P, diffusion, 0 );
    Grid::swap( grid.densG, grid.densG_P );
    diffuse( grid.densB, grid.densB_P, diffusion, 0 );
    Grid::swap( grid.densB, grid.densB_P );

    //advect( grid.densR, grid.densR_P, grid.velX, grid.velY, 0  );
    advectRGB(grid.densR, grid.densR_P, grid.densG, grid.densG_P, grid.densB, grid.densB_P, grid.velX, grid.velY, 0);
    stepCount++;
    grid.clearOld();
}

void FluidSystem::setup() {
    fixBoundary(grid.densR,0);
    fixBoundary(grid.densG,0);
    fixBoundary(grid.densB,0);
    fixBoundary(grid.velX,1);
    fixBoundary(grid.velY,2);
    fixBoundary(grid.densR_P,0);
    fixBoundary(grid.densG_P,0);
    fixBoundary(grid.densB_P,0);
    fixBoundary(grid.velX_P,1);
    fixBoundary(grid.velY_P,2);
    getPointsForScreen(particles, color, velocities, indices, vel_indices);
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
