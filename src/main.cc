#include <GL/glew.h>
#include "gui.h"
#include "OpenGLUtil.h"
#include "SystemHandler.h"

int window_width = 640, window_height = 640;

int main(int argc, char* argv[])
{
    OpenGLUtil openGL = OpenGLUtil(window_width, window_height, "Particles");
    GLFWwindow* window = openGL.setup();
    GUI gui(window);
    
    SystemHandler* systemHandler = new SystemHandler(window_height, window_width);
    gui.delegate = systemHandler;
    
	while (openGL.drawBool()) {
        openGL.beforeDraw();
        
        systemHandler->step();
        
        openGL.afterDraw();
	}
    openGL.destroy();
}
