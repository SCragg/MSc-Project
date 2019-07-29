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

// Include shader
#include "shader.h"

#include<utility>
#include <vector>
#include <string>




class GUI
{
private:
	float time = 0;
	int numofshaders;
	int currentshader = 0;
	const std::vector<Shader>& Shaders;

	void nextshader();

public:

	GUI(const std::vector<Shader>& shaderlist): Shaders(shaderlist) {};
	~GUI();

	static void Initialise(GLFWwindow* window);
	static void Cleanup();
	void Render();

	void set_time(float t) { time = t; }
	const float& get_time() { return time; }

	const int& get_currentshader() { return currentshader; }
};