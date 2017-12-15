//
//  Node.h
//  Particles
//
//  Created by Matthew Ruston on 12/14/17.
//

#ifndef Node_h
#define Node_h

#include "MassParticle.h"

struct Node {
    int nodeID = 0;
    
    int numberOfParticles = 0;
    //TODO: Add velocity
    
    void addParticle(MassParticle& p) {
        ++numberOfParticles;
    }
    
    void removeParticle(MassParticle& p) {
        --numberOfParticles;
    }
};

#endif /* Node_h */
