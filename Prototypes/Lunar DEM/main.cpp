/* 

Lunar DEM prototype - Sean Cragg 01/05/2019

main.cpp

//

*/

//Linking to libraries
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glload.lib")

#endif
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "SOIL.lib")

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "shader.h"
#include "DEM_terrain.h"
#include "cube.h"

//std lib includes
#include <iostream>
#include <stack>
#include <utility>

//Variable Declarations
/* Position and view globals */
GLfloat angle_x, angle_inc_x, model_scale; //from lab example
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z; //from lab example
GLfloat move_x, move_y, move_z;
GLfloat aspect_ratio;		// Aspect ratio of the window defined in the reshape callback

//Light rotation
GLfloat HourAngle; //Angle of light direction, specified in degrees

/* Uniforms*/
GLuint ubo_Matrices; //UBO for view, projection, model and normal transformation matrices

//Offsets for ubo
const GLint offset_projection = 0;
const GLint offset_view = 64;
const GLint offset_model = 128;
const GLint offset_normalmatrix = 192;

//Uniforms
GLuint lightdirID;
GLuint lamb_lightdirID;
GLuint therm1_lightdirID, therm1_albedoID, therm1_solarID, therm1_emissID;

/* Global instances of our objects */
Shader normalShader, cubeShader;
vector<Shader> terrainShaders;
DEM_terrain* LunarTerrain;
Cube aCube;

//Flags
GLboolean shownormals;
GLuint drawmode;
GLuint currentterrainshader;

using namespace std;
using namespace glm;

/*
This function is called before entering the main rendering loop. Initialisations.
*/
void init(GLWrapper *glw)
{
	/* Set the view transformation controls to their initial values*/
	angle_x = angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0;
	move_x = 0;
	move_y = -10;
	move_z = -50;

	model_scale = .2f;
	aspect_ratio = 1024.f / 768.f;	// Initial aspect ratio from window size - from lab examples

	//hour angle
	HourAngle = 0;

	//initial flag values
	shownormals = false;
	drawmode = 0;
	currentterrainshader = 0;

	//Create Lunar DEM
	LunarTerrain = new DEM_terrain(512, 512, "..\\..\\DEMs\\1\\surface_region_0_layer_0.dem", 1024, 1024); //had last two as 1024 for a bit for resolution but possibly need to readjust normal code
	LunarTerrain->generateTerrain();
	LunarTerrain->createObject();

	//Create cube
	aCube.makeCube();

	/* Load terrain shaders in to shader vector */
	try
	{
		terrainShaders.push_back(Shader("Hapke", "..\\..\\shaders\\Hapke.vert", "..\\..\\shaders\\Hapke.frag"));
	}
	
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	try
	{
		terrainShaders.push_back(Shader("Lambert", "..\\..\\shaders\\Hapke.vert", "..\\..\\shaders\\Lambert.frag"));
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	try
	{
		terrainShaders.push_back(Shader("Thermal", "..\\..\\shaders\\Thermal_2.vert", "..\\..\\shaders\\Thermal_2.frag"));
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	//Load other shaders
	try
	{
		normalShader.LoadShader("..\\..\\shaders\\show_normals.vert", "..\\..\\shaders\\show_normals.frag", "..\\..\\shaders\\show_normals.geom");
		normalShader.SetName("Normal");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	try
	{
		cubeShader.LoadShader("..\\..\\shaders\\Basic.vert", "..\\..\\shaders\\Basic.frag");
		cubeShader.SetName("Cube");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	//Uniform buffer setup
	//get uniform block index for shaders
	GLuint uniBlock_MatHapke = glGetUniformBlockIndex(terrainShaders[0].ID, "Matrices");
	GLuint uniBlock_MatLambert = glGetUniformBlockIndex(terrainShaders[1].ID, "Matrices");
	GLuint uniBlock_MatThermal_1 = glGetUniformBlockIndex(terrainShaders[2].ID, "Matrices");
	GLuint uniBlock_MatNormals = glGetUniformBlockIndex(normalShader.ID, "Matrices");
	GLuint uniBlock_MatCube = glGetUniformBlockIndex(cubeShader.ID, "Matrices");
	//then explicitly link shaders uniform block to binding point, 0 for this buffer
	glUniformBlockBinding(terrainShaders[0].ID, uniBlock_MatHapke, 0);
	glUniformBlockBinding(terrainShaders[1].ID, uniBlock_MatLambert, 0);
	glUniformBlockBinding(terrainShaders[2].ID, uniBlock_MatThermal_1, 0);
	glUniformBlockBinding(normalShader.ID, uniBlock_MatNormals, 0);
	glUniformBlockBinding(cubeShader.ID, uniBlock_MatCube, 0);
	//Create matrices uniform buffer object
	glGenBuffers(1, &ubo_Matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_Matrices);
	glBufferData(GL_UNIFORM_BUFFER, 240, NULL, GL_DYNAMIC_DRAW); //Buffer size of 240 bytes
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//bind buffer to binding point
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_Matrices);

	/* Define light position uniforms */
	lightdirID = glGetUniformLocation(terrainShaders[0].ID, "lightdir");
	lamb_lightdirID = glGetUniformLocation(terrainShaders[1].ID, "lightdir");
	therm1_lightdirID = glGetUniformLocation(terrainShaders[2].ID, "lightdir");

	//Define uniforms for thermal shader
	therm1_solarID = glGetUniformLocation(terrainShaders[2].ID, "solar_constant");
	therm1_emissID = glGetUniformLocation(terrainShaders[2].ID, "emissivity");
	therm1_albedoID = glGetUniformLocation(terrainShaders[2].ID, "albedo");
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
class because we registered display as a callback function */
void display(GUI* gui)
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	// Projection matrix : 30° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units - from lab example
	mat4 projection = perspective(radians(30.0f), aspect_ratio, 0.1f, 200.0f);

	// Camera matrix
	mat4 view = lookAt(
		vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
		vec3(0, 0, 0), // looks at the origin
		vec3(0, 1, 0)  // up is positive y.
	);

	view = translate(view, vec3(move_x, move_y, move_z));

	// Define light direction using hour angle
	vec4 lightdirection = rotate(mat4(1.0f), (gui->get_time() * 6.28318530718f) , vec3(0, 0, 1)) * vec4(0, 1, 0, 1);

	// Send our projection and view uniforms and light position to the shader
	terrainShaders[gui->get_currentshader()].use();
	switch (gui->get_currentshader())
	{
	case 0:
		glUniform4fv(lightdirID, 1, value_ptr(lightdirection));
		break;

	case 1:
		glUniform4fv(lamb_lightdirID, 1, value_ptr(lightdirection));
		break;

	case 2:
		glUniform4fv(therm1_lightdirID, 1, value_ptr(lightdirection));
		break;
	}

	//Update matrix UBO with projection and view matrices
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_Matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, offset_projection, sizeof(mat4), value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, offset_view, sizeof(mat4), value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));
	// Define the normal matrix
	mat3 normalmatrix;

	// Define our transformations that apply to terrain
	model.push(model.top());
	{
		model.top() = translate(model.top(), vec3(0, 0, 0));
		model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));//scale equally in all axis
		model.top() = rotate(model.top(), -radians(angle_x), vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
		model.top() = rotate(model.top(), -radians(angle_y), vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
		model.top() = rotate(model.top(), -radians(angle_z), vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
		normalmatrix = transpose(inverse(mat3(view * model.top())));

		//Update UBO with model and normal matrices
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_Matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, offset_model, sizeof(mat4), &model.top()[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset_normalmatrix, sizeof(vec4)*3, value_ptr(mat4(normalmatrix)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Draw terrain
		terrainShaders[gui->get_currentshader()].use();
		if (gui->get_currentshader() == 2)
		{
			//If thermal shader send uniforms:
			glUniform1f(therm1_albedoID, 0.08); //Albedo of 0.08
			glUniform1f(therm1_emissID, 0.95); //Emissivity of 0.95
			glUniform1f(therm1_solarID, 1370); //Solar Constant 1370

		}
		LunarTerrain->drawTerrain(drawmode);

		if (shownormals)
		{
			normalShader.use();
			LunarTerrain->drawTerrain(2); //draw as points
		}

	}
	model.pop();

	/*
	model.push(model.top());
	{
		model.top() = translate(model.top(), vec3(lightx, lighty, lightz));
		model.top() = scale(model.top(), vec3(1, 1, 1));
		normalmatrix = transpose(inverse(mat3(view * model.top())));

		//Update UBO
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_Matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, offset_model, sizeof(mat4), &model.top()[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offset_normalmatrix, sizeof(mat3), &normalmatrix);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		//draw cube
		cubeShader.use();
		aCube.drawCube(cubeShader);
	}
	model.pop();
	*/

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	////////////////Modify our animation variables///////////////////

	//taken from class example, spins around the whole model.
	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;

}

/* Called whenever the window is resized. The new window size is given, in pixels. taken from lab examples */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// Store aspect ratio to use for our perspective projection
	aspect_ratio = float(w) / float(h);
}

/* change view angle, exit upon ESC taken from lab lab examples*/
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	//Closes if escape is pressed, from lab example
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//Controls model rotation and scale, adapted from class example
	if (key == 'Q') angle_inc_x -= 0.1f;
	if (key == 'Q' && action == GLFW_RELEASE) angle_inc_x = 0;
	if (key == 'W') angle_inc_x += 0.1f;
	if (key == 'W' && action == GLFW_RELEASE) angle_inc_x = 0;
	if (key == 'E') angle_inc_y -= 0.1f;
	if (key == 'E' && action == GLFW_RELEASE) angle_inc_y = 0;
	if (key == 'R') angle_inc_y += 0.1f;
	if (key == 'R' && action == GLFW_RELEASE) angle_inc_y = 0;
	if (key == 'T') angle_inc_z -= 0.1f;
	if (key == 'T' && action == GLFW_RELEASE) angle_inc_z = 0;
	if (key == 'Y') angle_inc_z += 0.1f;
	if (key == 'Y' && action == GLFW_RELEASE) angle_inc_z = 0;
	if (key == 'A') model_scale -= 0.004f;
	if (key == 'S') model_scale += 0.004f;

	//Moves camera along x, y, z axes
	if (key == GLFW_KEY_UP) move_y -= 1.0;
	if (key == GLFW_KEY_DOWN) move_y += 1.0;
	if (key == GLFW_KEY_LEFT) move_x += 1.0;
	if (key == GLFW_KEY_RIGHT) move_x -= 1.0;
	if (key == GLFW_KEY_RIGHT_SHIFT) move_z -= 1.0;
	if (key == GLFW_KEY_RIGHT_CONTROL) move_z += 1.0;

	//Change hour angle
	if (key == 'O')
	{
		if (HourAngle < 359) HourAngle++;
		else HourAngle = 0;
		cout << "Hour angle: " << HourAngle << " degrees. \n";
	}

	if (key == 'P')
	{
		if (HourAngle > 0) HourAngle--;
		else HourAngle = 359;
		cout << "Hour angle: " << HourAngle << " degrees. \n";
	}

	//Shows/hides normal
	if (key == 'V' && action == GLFW_PRESS)
	{
		if (shownormals == false) shownormals = true;
		else shownormals = false;
	}

	//Change drawmode
	if (key == 'B' && action == GLFW_PRESS)
	{
		if (drawmode < 2) drawmode++;
		else drawmode = 0;
	}

	//Change shader
	if (key == 'C' && action == GLFW_PRESS)
	{
		if (currentterrainshader < terrainShaders.size() - 1) currentterrainshader ++;
		else  currentterrainshader = 0;
	}
}


/* Entry point of program */
int main(int argc, char* argv[])
{
	GUI *gui = new GUI(terrainShaders);
	GLWrapper *glw = new GLWrapper(1024, 768, "Lunar DEM", gui);

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	// Register the callback functions
	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	/* Output the OpenGL vendor and version */
	glw->DisplayVersion();

	init(glw);
	GUI::Initialise(glw->getWindow());
	
	glw->eventLoop();
	
	GUI::Cleanup();
	delete(glw);
	
	return 0;
}
