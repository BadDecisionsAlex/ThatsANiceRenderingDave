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

const char* fluid_geometry_shader =
#include "shaders/fluid.geom"
;

const char* particle_fragment_shader =
#include "shaders/particle.frag"
;
