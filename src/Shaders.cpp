//
//  Shaders.cpp
//  Particles
//
//  Created by Matthew Ruston on 12/1/17.
//

#include "Shaders.h"

const char* particle_vertex_shader =
#include "shaders/particle.vert"
;

const char* fluid_vertex_shader =
#include "shaders/fluid.vert"
;

const char* particle_geometry_shader =
#include "shaders/particle.geom"
;

const char* fluid_geometry_shader =
#include "shaders/fluid.geom"
;

const char* collision_geometry_shader =
#include "shaders/collision.geom"
;

const char* particle_fragment_shader =
#include "shaders/particle.frag"
;

const char* smoke_vertex_shader =
#include "shaders/smoke.vert"
;

const char* smoke_geometry_shader =
#include "shaders/smoke.geom"
;

const char* smoke_fragment_shader =
#include "shaders/smoke.frag"
;

const char* fluid_fragment_shader = 
#include "shaders/fluid.frag"
;

const char* rainbow_particles_geometry_shader =
#include "shaders/rainbowParticles.geom"
;

const char* rainbow_particles_fragment_shader =
#include "shaders/rainbowParticles.frag"
;

const char* grid_vertex_shader =
#include "shaders/grid.vert"
;

const char* grid_geometry_shader =
#include "shaders/grid.geom"
;

const char* grid_fragment_shader =
#include "shaders/grid.frag"
;

const char* velocity_vertex_shader = 
#include "shaders/velfield.vert"
;

const char* velocity_geometry_shader = 
#include "shaders/velfield.geom"
;

const char* velocity_fragment_shader = 
#include "shaders/velfield.frag"
;
