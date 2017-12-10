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

const char* particle_geometry_shader =
#include "shaders/particle.geom"
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


const char* grid_vertex_shader =
#include "shaders/grid.vert"
;

const char* grid_geometry_shader =
#include "shaders/grid.geom"
;

const char* grid_fragment_shader =
#include "shaders/grid.frag"
;
