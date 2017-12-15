//
//  GridSystemHandler.h
//  Particles
//
//  Created by Matthew Ruston on 12/10/17.
//

#ifndef GridSystemHandler_h
#define GridSystemHandler_h

#include <stdio.h>
#include "ParticleSystem.h"
#include "SmokeSystem.h"
#include "SpaceSystem.h"

//Im using this to easily switch out the class type.
//There are certainly better ways to make this abstract
#define SYSTEM SpaceSystem

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

class GridSystemHandler: public ParticleSystem {
private:
    //TODO: Make a good way to get any type of system's particles
     SYSTEM *system;
    
    Node outsideGridNode;
    
    int nodeSize = 10;
    int rows = 0;
    int columns = 0;
    vector<vector<Node>> grid;
    
    vec4 toScreen(const vec3& point);
    void getPointsForScreen(vector<vec4>& points, vector<uvec3>& indices);
    
    //Rendering (Could be made simpler)
    RenderDataInput particle_pass_input;
    RenderPass particle_pass;
    vector<vec4> points;
    vector<uvec3> indices;
    
public:
    
    GridSystemHandler(SYSTEM* _system);
    
    ~GridSystemHandler() {
        free(system);
    }
    
    void draw();
    void prepareDraw();
    
    void step() {
        system->step();
        
        for (MassParticle p : system->particles) {
            Node& node = nodeForPosition(p.p);
            vec3 old_p = p.p - p.velocity;
            Node& oldNode = nodeForPosition(old_p);
            if (node.nodeID != oldNode.nodeID) {
                oldNode.removeParticle(p);
                node.addParticle(p);
            }
        }
    }
    
    Node& nodeForPosition(vec3& p) {
        int r = (int)(p.y / nodeSize);
        int c = (int)(p.x / nodeSize);
        
        //TODO: Redo the positioning of the particle worlds to have 0,0 always be the center, not the lower corner
        
        if (r < 0 || r >= rows || c < 0 || c >= columns) {
            return outsideGridNode;
        }
        
        return grid[r][c];
    }
    
    void setup() {
        outsideGridNode.nodeID = -1;
        columns = (int)(width / nodeSize) + 1;
        rows = (int)(height / nodeSize) + 1;
        
        int count = 0;
        for (int r = 0; r < rows; ++r) {
            vector<Node> column;
            for (int c = 0; c < columns; ++c) {
                Node node;
                node.nodeID = count++;
                column.push_back(node);
            }
            grid.push_back(column);
        }
        
        system->height = height;
        system->width = width;
        system->setup();
        
        for (MassParticle p : system->particles) {
            Node& node = nodeForPosition(p.p);
            node.addParticle(p);
        }
        
        getPointsForScreen(points, indices);
    }
};

#endif /* GridSystemHandler_hpp */
