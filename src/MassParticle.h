//
//  MassParticle.hpp
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef MassParticle_hpp
#define MassParticle_hpp

#include <stdio.h>
#include "AbstractParticle.h"

struct MassParticle: AbstractParticle {
    vec3 velocity;
    int mass = 1;
    
    MassParticle(vec3 position) {
        p = position;
    };
};

#endif /* MassParticle_hpp */
