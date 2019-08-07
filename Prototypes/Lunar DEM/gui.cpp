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

		if (ImGui::Button("Change shader"))
		{
			nextshader();
		}
		//Insert code to deal with shader changing
		ImGui::Text(Shaders[currentshader].GetName().c_str());

		/*
		//if current shader is thermal include button to change colour type

		if (current_shader == thermal)
		{
			if (ImGui::Button(colourmode))
				//switch colourmode
		}
		*/

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