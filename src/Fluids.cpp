//
// Created by Joshua Cristol on 12/2/17.
//

#include "Fluids.h"

// applies a force to all of the cells of the grid ( useful for gravity )
void FluidSystem::add ( vec2 force, float dt ) {
    for(Cell& c : grid)
        c.velocity += dt * force;
}

void FluidSystem::diffuse ( float diff, float dt) {
    float a = dt * diff * grid.N * grid.N;
    for( int k=0; k < 20; ++k ){
        for( int r=0; r < grid.N; ++r ){
            for( int c=0; c < grid.N; ++c ){
                grid.at(r,c).velocity = ( 1.0f / ( 1.0f + 4.0f * a ) )
                                 * ( grid.at(r,c).oldVelocity + a
                                                       * ( grid.at(r-1,c).oldVelocity + grid.at(r+1,c).oldVelocity + grid.at(r,c-1).oldVelocity + grid.at(r,c+1).oldVelocity ) );
            }}}
}

// the psuedo code was pretty easy to parse into C++ until this point.
// this is a near copy of their version until I can understand it more and swap for better structures and library functions.
// FIXME
void FluidSystem::advect ( float dt ) {
    int r0, c0, r1, c1;         // i have no idea what these are.
    float s0, t0, s1, t1;       // these either.
    float dt0 = dt * float(grid.N);
    for( int r=0; r < grid.N; ++r ){
        for( int c=0; c < grid.N; ++c ){
            vec2 pos = vec2( r, c );
            pos -=  dt0 * grid.at(r,c).velocity;
            //  clamp between ( 0.5, N + 0.5 )
            if( pos.x < 0.5 )
                pos.x = 0.5;
            if( pos.x > grid.N + 0.5 )
                pos.x = grid.N + 0.5;
            if( pos.y < 0.5 )
                pos.y = 0.5;
            if( pos.y > grid.N + 0.5 )
                pos.y = grid.N + 0.5;
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
            grid.at(r,c).density = s0 * ( t0 * grid.at(r0,c0).oldDensity + t1 * grid.at(r0,c1).oldDensity + s1 * grid.at(r1,c0).oldDensity + t1 * grid.at(r1,c1).oldDensity );
        }}
}

// definitely make those inputs static to optimize but I'm leaving them for now.
// FIXME
void FluidSystem::density_step( float diff, float dt ){
    add( vec2( 0.0, -9.8 ), dt );
    for( Cell& c : grid )   // put current velocity into previous velocity.
        c.oldVelocity = c.velocity;
    diffuse( diff, dt );
    for( Cell& c : grid )   // put current velocity into previous velocity.
        c.oldVelocity = c.velocity;
    advect( dt );
}

// divergence parameter shows how velocity fields move "along themselves" flowing through the grid.
void FluidSystem::project( float div ){
    float h = 1.0 / grid.N;
    for( int r=0; r < grid.N; ++r ){
        for( int c=0; c < grid.N; ++c ){
            grid.at(r,c).divergence = -0.5f * h * ( grid.at(r-1,c).oldVelocity.x - grid.at(r+1,c).oldVelocity.x + grid.at(r,c+1).oldVelocity.y - grid.at(r,c-1).oldVelocity.y );
        }
    }
}

void FluidSystem::velocity_step( float visc, float dt ){
    // FIXME
}
