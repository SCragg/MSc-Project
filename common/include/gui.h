/*
gui.h

Singleton class for GUI

Uses Dear ImGUI (https://github.com/ocornut/imgui)

Static functions used for initialisation and clean up of all GUI objects.

Render function used to draw gui, all required code should be placed in the render function,
to draw GUI place render function in the draw loop

*/

#pragma once

// Include Dear ImGUI
#include <ImGUI/imgui.h>
#include  <ImGUI/imgui_impl_glfw.h>
#include  <ImGUI/imgui_impl_opengl3.h>

#include "wrapper_glfw.h"

class GUI
{
private:
	float time = 0;
	int numofshaders;
	int currentshader = 0;


public:

	static void Initialise(GLFWwindow* window);
	static void Cleanup();
	void Render();
};