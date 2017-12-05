#ifndef SKINNING_GUI_H
#define SKINNING_GUI_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "GUIDelegate.h"
#include <vector>

class GUI {
public:
	GUI(GLFWwindow*);
	~GUI();

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    std::vector<GUIDelegate *> delegates;
    
private:
	GLFWwindow* window_;

	int window_width_, window_height_;

	bool drag_state_ = false;
	int current_button_ = -1;
    
	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;

};

#endif
