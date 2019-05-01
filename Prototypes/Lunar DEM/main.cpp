/* 

Lunar DEM prototype - Sean Cragg 01/05/2019

main.cpp

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
#include <iostream>
#include <stack>
/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "shader.h"

//Variable Declarations
/* Position and view globals */
GLfloat angle_x, angle_inc_x, model_scale; //from lab example
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z; //from lab example
GLfloat move_x, move_y, move_z;

//View Globals
GLfloat aspect_ratio;		// Aspect ratio of the window defined in the reshape callback
GLfloat lightx, lighty, lightz;


/* Uniforms*/
GLuint modelID, viewID, projectionID, normalmatrixID,
	lightposID, diffuseID, shininessID, is_model_texturedID;

/* Global instances of our objects */
Shader aShader;

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
	move_x = move_y = move_z = 0;

	model_scale = 1.f;
	aspect_ratio = 1024.f / 768.f;	// Initial aspect ratio from window size - from lab examples


	//light position values
	lightx = 0.5;
	lighty = 0.5;
	lightz = 0.5;

	/* Load shaders in to shader object */

	try
	{
		aShader.LoadShader("..\\..\\shaders\\Assign1.vert", "..\\..\\shaders\\Assign1.frag");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	/* Define uniforms to send to vertex shader */
	modelID = glGetUniformLocation(aShader.ID, "model");
	viewID = glGetUniformLocation(aShader.ID, "view");
	projectionID = glGetUniformLocation(aShader.ID, "projection");
	lightposID = glGetUniformLocation(aShader.ID, "lightpos");
	normalmatrixID = glGetUniformLocation(aShader.ID, "normalmatrix");
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	//Use our shader
	aShader.use();

	// Projection matrix : 30° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units - from lab example
	mat4 projection = perspective(radians(30.0f), aspect_ratio, 0.1f, 100.0f);

	// Camera matrix
	mat4 view = lookAt(
		vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
		vec3(0, 0, 0), // looks at the origin
		vec3(0, 1, 0)  // up is positive y.
	);

	view = translate(view, vec3(move_x, move_y, move_z));

	// Define the light position and transform by the view matrix - from lab example
	vec4 lightpos = view * vec4(lightx, lighty, lightz, 1.0);

	// Send our projection and view uniforms and light position to the shader
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniform4fv(lightposID, 1, value_ptr(lightpos));

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));
	// Define the normal matrix
	mat3 normalmatrix;

	// Define our transformations that apply to all our objects expecpt light cube
	model.top() = translate(model.top(), vec3(0, 0, 0));
	model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));//scale equally in all axis
	model.top() = rotate(model.top(), -radians(angle_x), vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model.top() = rotate(model.top(), -radians(angle_y), vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model.top() = rotate(model.top(), -radians(angle_z), vec3(0, 0, 1)); //rotating in clockwise direction around z-axis


	/*
	Example of draw call include these here.

	model.push(model.top());
	{
		model.top() = translate(model.top(), vec3(1.175, -0.295, 0.58)); // Values just worked out after trial and error to make them fit
		model.top() = rotate(model.top(), -radians(needle_rotation), vec3(0, 0, 1)); //rotating around z-axis
		//Send uniforms and draw Needle
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);
		Needle.Draw(aShader);
	}
	model.pop();
	*/


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
	if (key == 'A') model_scale -= 0.04f;
	if (key == 'S') model_scale += 0.04f;

	//Moves camera along x, y, z axes
	if (key == GLFW_KEY_UP) move_y -= 0.02;
	if (key == GLFW_KEY_DOWN) move_y += 0.02;
	if (key == GLFW_KEY_LEFT) move_x += 0.02;
	if (key == GLFW_KEY_RIGHT) move_x -= 0.02;
	if (key == GLFW_KEY_RIGHT_SHIFT) move_z -= 0.02;
	if (key == GLFW_KEY_RIGHT_CONTROL) move_z += 0.02;

	//Move light position
	//if (action != GLFW_PRESS) return;
	if (key == 'O') lightx += 0.02f;
	if (key == 'P') lightx -= 0.02f;
	if (key == 'K') lighty += 0.02f;
	if (key == 'L') lighty -= 0.02f;
	if (key == 'N')	lightz += 0.02f;
	if (key == 'M') lightz -= 0.02f;
}


/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper *glw = new GLWrapper(1024, 768, "Gramophone");;

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

	glw->eventLoop();

	delete(glw);
	return 0;
}
