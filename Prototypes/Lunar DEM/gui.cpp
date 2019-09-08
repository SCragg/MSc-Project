/*
	gui.cpp - Sean Cragg
*/
#include "gui.h"

void GUI::Initialise(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void GUI::Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::Render()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Place imGUI draw functions here before ImGui::Render()
	{
		ImGui::Begin("Controls");

		ImGui::SliderFloat("Time", &time, 0.0f, 1.0f);
		ImGui::Checkbox("Auto time", &auto_time);
		if (auto_time)
		{
			ImGui::SliderFloat("Speed", &auto_speed, 0.0f, 0.015f);
			time += auto_speed;
			if (time > 1) time -= 1;
		}

		if (ImGui::Button("Change shader"))
		{
			nextshader();
		}
		//Insert code to deal with shader changing
		ImGui::Text(Shaders[currentshader].GetName().c_str());

		
		//if current shader is thermal include button to change colour type

		if (Shaders[currentshader].GetName() == "Thermal")
		{
			if (ImGui::Button("Swap colourmode"))
			{
				colourmode++;
				if (colourmode == NUM_OF_COLOURMODES) colourmode = 0;
			}
		}
		

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		//bool showwindow = true;
		//ImGui::ShowDemoWindow(&showwindow);
	}

	//Gui functions before here
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::nextshader()
{
	if (currentshader < Shaders.size() - 1) currentshader++;
	else currentshader = 0;
}