// Comment out the line below to add this bad boy to the compile chain or to see the code with proper color fields.
// Removed from compile chain until it is complete.
//#if 0

#ifndef FLUIDS_H
#define FLUIDS_H

#include "ParticleSystem.h"
#include <glm/glm.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>

using glm::vec2;
using glm::ivec2;
using glm::uvec1;
using glm::uvec2;
using glm::vec1;
using std::vector;
using std::size_t;
using std::ostream;

// *****
// GRID
// *****

struct Grid{

    // Member Variables
    float*  densR;
    float* densR_P;
    float* densG;
    float* densG_P;
    float* densB;
    float* densB_P;
    float* velX;
    float* velX_P;
    float* velY;
    float* velY_P;

    // Class Variables
    int N;

    Grid( int n_ = 64 ) : N(n_), 
        densR(new float[(n_+2)*(n_+2)]), 
        densR_P(new float[(n_+2)*(n_+2)]), 
        densG(new float[(n_+2)*(n_+2)]), 
        densG_P(new float[(n_+2)*(n_+2)]), 
        densB(new float[(n_+2)*(n_+2)]), 
        densB_P(new float[(n_+2)*(n_+2)]), 
        velX(new float[(n_+2)*(n_+2)]), 
        velX_P(new float[(n_+2)*(n_+2)]), 
        velY(new float[(n_+2)*(n_+2)]), 
        velY_P(new float[(n_+2)*(n_+2)]) {
        clear(); }


    // Functions
    void free( float*& x ){
        delete[] x;
        x = new float[(N+2)*(N+2)];
    }

    void clear( float*& x){
        for(int i = 0; i < (N+2)*(N+2); ++i)
            x[i] = 0.0f;
    }

    void clear(){
        for( int i = 0; i < (N+2)*(N+2); ++i )
            densR[i] = densG[i] = densB[i] = velX[i] = velY[i] = densR_P[i] = densG_P[i] = densB_P[i] = velX_P[i] = velY_P[i] = 0.0f;
    }

    void clearNew(){
        for( int i = 0; i < (N+2)*(N+2); ++i )
            densR[i] = densG[i] = densB[i] = velX[i] = velY[i] = 0.0f;
    }

    void clearOld(){
        for( int i = 0; i < (N+2)*(N+2); ++i )
            densR_P[i] = densG_P[i] = densB_P[i] = velX_P[i] = velY_P[i] = 0.0f;
    }

    float& at( float*& x,  int r, int c ) { return x[ r * (N+2) + c ]; }
    const float& at( float*& x, int r, int c ) const { return x[ r * (N+2) + c]; }

    static void swap( float*& a, float*& b ){
        float* t = a;
        a = b;
        b = t;
    }

    ivec2 iToCo( int i ) { return ivec2( i / (N+2), i % (N+2)); }
    int coToI( int r, int c ) { return r * (N+2) + c; }
    int coToI( ivec2 v )  { return coToI( v.x, v.y ); }
    vec2 iToPos( int i ) { return coToPos(iToCo(i)); }
    vec2 coToPos( int r, int c ) { return vec2( (float(r) + 0.5f), (float(c) + 0.5f)); }
    vec2 coToPos( ivec2 v ) { return coToPos( v.x, v.y ); }
    int posToI( float x, float y ) { return coToI(int(x), int(y)); }
    int posToI( vec2 v ){return posToI( v.x, v.y );}
    ivec2 posToCo( float x, float y ) { return ivec2( int(x), int(y)); }
    ivec2 posToCo( vec2 v ) { return posToCo( v.x, v.y ); }

};

class FluidSystem : public ParticleSystem {
public:
    FluidSystem(int grid_size=64, float time_step=(1.0f/60.0f), float diff_=0.01f, float visc_=0.6f );

    // Draw Functions
    void step();
    void setup();
    void prepareDraw();
    void draw();
    void keyWasPressed(int keyCode);
    void mouseDragged(float x, float y);
    void mouseStateChange(bool dragging) {
        isDragging = dragging; }
    void mouseButton(int button){
        mouse_button = button;
    }

    Grid grid;

private:
    // Member Variables
    float dt;
    float diffusion;
    float viscosity;

    // For UI
    vec3 mouse;
    vec3 mouse0;
    bool isDragging = false;
    int mouse_button;

    // Functions
    
    // mixes the value of var in src to var in "grid"
    void advect( float*& d, float*& d0, float*& u, float*& v, short f );
    // mixes varB into varA applying scalar
    void diffuse( float*& x, float*& x0, float scalar, short f );
    // corrects divergence in flows
    void project( float*& u, float*& v, float*& p, float*& div );
    // Helper for Diffuse and Project : applies a linear interpolation between neighboring cells from srcB.varB to srcA.varB using two scalars.
    void linearSolver( float*& x, float*& x0, float scalarNumerator, float scalarDenominator, short f );
    // Applies var from the body to the boundary cells of src
    void fixBoundary( float*& x, short f );
    // applies force/scalar from srcB to varA in srcA and applies dt
    void add( float*& x, float*& s );
    // moves a value from grid to oldGrid
    
    // Drawing
    void getPointsForScreen(vector<vec4>& particles, vector<vec4>& velocities, vector<uvec3>& indices, vector<uvec2>& vel_indices);
    vec4 toScreen(const vec3& particle);
    vec4 toScreen(const vec2& pos);

    //Rendering (Could be made simpler)
    RenderDataInput fluid_pass_input;
    RenderDataInput velocity_pass_input;
    RenderPass fluid_pass;
    RenderPass velocity_pass;
    vector<vec4> particles;
    vector<vec4> velocities;
    vector<uvec2> vel_indices;
    vector<uvec3> indices;
};

#endif
