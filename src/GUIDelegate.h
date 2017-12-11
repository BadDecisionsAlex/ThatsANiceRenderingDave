//
//  GUIDelegate.h
//  Particles
//
//  Created by Matthew Ruston on 12/4/17.
//

#ifndef GUIDelegate_h
#define GUIDelegate_h

struct GUIDelegate {
    virtual void keyWasPressed(int keyCode) {}
    
    virtual void mouseDragged(float x, float y) {}
    
    virtual void mouseStateChange(bool dragging) {}

    virtual void mouseButton(int button){}
};

#endif /* GUIDelegate_h */
