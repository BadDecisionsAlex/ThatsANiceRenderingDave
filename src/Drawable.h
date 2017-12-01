//
//  Drawable.h
//  Particles
//
//  Created by Matthew Ruston on 11/30/17.
//

#ifndef Drawable_h
#define Drawable_h
#include <GL/glew.h>
#include <debuggl.h>
#include <iostream>
#include "render_pass.h"

struct Drawable {
    void virtual prepareDraw() = 0;
    void virtual draw() = 0;
};

#endif /* Drawable_h */
