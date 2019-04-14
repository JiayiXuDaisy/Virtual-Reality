/*
SJTU - CS230/CS238: Virtual Reality

Homework 1:		 Hello Teapot
Autor:			 Jiayi Xu
Student Number:	 516021910396
Class Number:	 F1603304

Tasks:
1. compile and get to run
2. wireframe teapot
3. write mouse and keyboard interaction

	Egemen Ertugrul
	egertu@sjtu.edu.cn
*/

//-----------------------------------------------------------------------------
// includes

#include <stdio.h>
#include <cstdlib>
#include <sdlWrapper.h>
#include <shader.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <model.h>

#undef main // Needed because SDL defines its own main
#ifdef WIN32
	#include <windows.h>
#endif

//-----------------------------------------------------------------------------
// some global variables

// Relative path to source files
std::string codeDir = "../";

// Relative path to models directory
std::string modelsDir = codeDir + "models/";

// flag to check if render wireframe or filled
bool	bWireframe = true;

// near clipping plane
float	zNear = 1.0;

// far clipping plane
float	zFar = 100000.0;

// Function Declarations
bool handleEvents(SDL_Event & evt, sdlWrapper & sdlContext);

// ****************************************************************************
// ************            Insert your code here                   ************
// ****************************************************************************

// You can use these global variables to start thinking about how to implement mouse movements
// You do not have to use these if you don't want

// parameters for the navigation
glm::vec3	viewerPosition	(0.0, 0.0, 100.0);
glm::vec3	viewerCenter	(0.0, 0.0, 0.0);
glm::vec3	viewerUp		(0.0, 1.0, 0.0);

// navigation values
float	navigationRotation[3] = { 0.0, 0.0, 0.0 };	//Record previous rotation angle of 3 axises
float   navigationPosition[3] = { 0.0, 0.0, 0.0 };	//Record previous position of 3 axises					

// position of the mouse when pressed
int		mousePressedX = 0, mousePressedY = 0;

//offset of the mouse from where it is pressed 
float	lastXOffset = 0.0, lastYOffset = 0.0, lastZOffset = 0.0;

// mouse button states
int		leftMouseButtonActive = 0, middleMouseButtonActive = 0, rightMouseButtonActive = 0;

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


//---------------------------------------------------------------
// main function
//---------------------------------------------------------------

void main(int argc, char **argv) {
	
	sdlWrapper sdlContext(1280, 960, "Hello World", 0, false);
	SDL_Event evt;

	float aspectRatio = (float)sdlContext.getWidth() / (float)sdlContext.getHeight();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	printf("EE 267 HW1\n\n");
	printf("keys:\n\tf\t- toggle fullscreen\n\tesc\t- exit\n\n");
	printf("mouse:\n\tleft button\t- rotation\n\tmiddle button\t- panning\n\tright button\t- zoom in and out\n");

	// Shader class instantiation
	Shader shader("../shaders/vertexShader.glsl", "../shaders/fragShader.glsl");

	// ****************************************************************************
	// ************************* Load Your Models Here ****************************
	// ****************************************************************************
	std::cout << "Loading Models ...";
	Model teapot(modelsDir + "teapot.obj");
	Model axes(modelsDir + "axes.obj");
	Model baymax(modelsDir + "baymax.obj");
	Model rose(modelsDir + "rose.obj");
	std::cout << " Done!" << std::endl;
	// ****************************************************************************
	// ****************************************************************************
	// ****************************************************************************

	//declaration of Model matrix, View matrix, Prejection matrix, Position vector 
	glm::mat4 projMat;	//universial projection matrix 
	glm::mat4 viewMat;	//universial view matrix

	glm::vec3 axisPos;	//position vector of axis
	glm::mat4 axisMat;	//model matrix of axis	

	glm::vec3 teapotPos;	//position vector of the middle teapot
	glm::vec3 baymaxPos;	//position vector of the right teapot
	glm::vec3 rosePos;	//position vector of the left teapot
	glm::mat4 teapotMat;	//model matrix of the middle teapot
	glm::mat4 baymaxMat;	//model matrix of the right teapot
	glm::mat4 roseMat;	//model matrix of the left teapot

	//definition of Model matrix, View matrix, Prejection matrix, Position vector 
	projMat = glm::perspective(glm::radians(20.0f), aspectRatio, zNear, zFar);
	viewMat = glm::lookAt(viewerPosition, viewerCenter, viewerUp);

	axisPos = glm::vec3(0.0f, 0.0f, 0.0f);
	axisMat = glm::scale(glm::translate(glm::mat4(1.0f), axisPos), glm::vec3(10.0f));

	teapotPos = glm::vec3(0.0f, 0.0f, 0.0f);
	teapotMat = glm::scale(glm::translate(glm::mat4(1.0f), teapotPos), glm::vec3(10.0f));

	// Main rendering loop
	bool running = true;
	while (running) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		running = handleEvents(evt, sdlContext);

		glViewport(0, 0, sdlContext.getWidth(), sdlContext.getHeight());

		// Always call this before you attach something to uniform, or initiate any draw calls
		shader.Use();
		
		// ****************************************************************************
		// ************************** Insert your Code here ***************************
		// ****************************************************************************
		
		// Attached Projection, Model, and View matricies to the shader for axis
		// In the shader the Proj * View * Model * vertex_coord operation is carried out
		shader.attachToUniform("Proj", projMat);
		shader.attachToUniform("View", viewMat);
		shader.attachToUniform("Model", axisMat);
		axes.Draw(shader);

		//task 1: with midlle button pressed, panning
		if (middleMouseButtonActive)
		{
			//panning & z-translation
			teapotPos = glm::vec3(navigationPosition[0] + lastXOffset/20, navigationPosition[1] - lastYOffset/20, navigationPosition[2]);	
			teapotMat = glm::scale(glm::translate(glm::mat4(1.0f), teapotPos), glm::vec3(10.0f));	
			//rotate for y-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[0]), glm::vec3(0.0, 1.0, 0.0));				
			//rotate for x-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[1]), glm::vec3(1.0, 0.0, 0.0));					
		}

		//task 2: with right button pressed, right -> backward, left -> forward
		if (rightMouseButtonActive)
		{
			//panning & z-translation
			teapotPos = glm::vec3(navigationPosition[0], navigationPosition[1], navigationPosition[2] + lastXOffset/10);					
			teapotMat = glm::scale(glm::translate(glm::mat4(1.0f), teapotPos), glm::vec3(10.0f));
			//rotate for y-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[0]), glm::vec3(0.0, 1.0, 0.0));				
			//rotate for x-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[1]), glm::vec3(1.0, 0.0, 0.0));					
		}

		//task 3: with left button pressed, x -> y-axis, y -> x-axis
		if (leftMouseButtonActive)
		{
			//panning & z-translation
			teapotPos = glm::vec3(navigationPosition[0], navigationPosition[1], navigationPosition[2]);									
			teapotMat = glm::scale(glm::translate(glm::mat4(1.0f), teapotPos), glm::vec3(10.0f));
			//rotate for y-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[0] + lastXOffset/5), glm::vec3(0.0, 1.0, 0.0));	
			//rotate for x-axis
			teapotMat = glm::rotate(teapotMat, glm::radians(navigationRotation[1] + lastYOffset/5), glm::vec3(1.0, 0.0, 0.0));	
		}


		baymaxPos = glm::vec3(2.0f, -0.8f, 0.0f);	//position translation for baymax model
		rosePos = glm::vec3(-2.0f, -0.8f, 0.0f);	//position translation for rose model
		baymaxMat = glm::scale(glm::translate(teapotMat, baymaxPos), glm::vec3(0.02f));	//scaling model matrix for baymax
		roseMat = glm::scale(glm::translate(teapotMat, rosePos), glm::vec3(0.02f));		//scaling model matrix for rose


		// Attached Projection, Model, and View matricies to the shader for teapot, baymax, rose
		// In the shader the Proj * View * Model * vertex_coord operation is carried out

		shader.attachToUniform("Model", teapotMat);
		teapot.Draw(shader);

		shader.attachToUniform("Model", baymaxMat);
		baymax.Draw(shader);

		shader.attachToUniform("Model", roseMat);
		rose.Draw(shader);

		//****************************************************************************
		// ****************************************************************************
		// ****************************************************************************
		
		sdlContext.swapbuffer();
	}

		
}

//**************************************************************
// SDL event handler function
//**************************************************************

bool handleEvents(SDL_Event & evt, sdlWrapper & sdlContext) {

	// Poll all events that have occurred
	while (SDL_PollEvent(&evt)) {
		// If Quit ( X in window is pressed)
		if (evt.type == SDL_QUIT) {
			return false;
		}

		// Handle Keyboard events
		if (evt.type == SDL_KEYDOWN) {
			// Quit if escape key is pressed
			if (evt.key.keysym.sym == SDLK_ESCAPE) {
				return false;
			}

			// Toggle Fullscreen
			if (evt.key.keysym.sym == SDLK_f) {
				sdlContext.toggleFullScreen();
			}

			// Toggle Wireframe
			if (evt.key.keysym.sym == SDLK_w) {
				if (bWireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					bWireframe = false;
				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					bWireframe = true;
				}
			}
		}

		// ****************************************************************************
		// ************************** Insert your Code here ***************************
		// ****************************************************************************

		// Handle Mouse Click Events
		if (evt.type == SDL_MOUSEBUTTONDOWN) {
			switch (evt.button.button) {
			case SDL_BUTTON_LEFT:
			{	
				leftMouseButtonActive = 1;
				mousePressedX = evt.motion.x;
				mousePressedY = evt.motion.y;
				printf("Left Button is pressed\n");
				break;
			}
			case SDL_BUTTON_RIGHT:	
			{
				rightMouseButtonActive = 1;
				mousePressedX = evt.motion.x;
				mousePressedY = evt.motion.y;
				printf("Right Button is pressed\n");
				break;
			}
			case SDL_BUTTON_MIDDLE:	
			{
				middleMouseButtonActive = 1;
				mousePressedX = evt.motion.x;
				mousePressedY = evt.motion.y;
				printf("Middle Button is pressed\n");
				break;
			}
			}
		}

		if (evt.type == SDL_MOUSEBUTTONUP) {
			switch (evt.button.button) {
			case SDL_BUTTON_LEFT:	 
			{
				leftMouseButtonActive = 0;
				printf("Left Button is released\n");
				//update previous rotation angles
				navigationRotation[0] += lastXOffset / 5;
				navigationRotation[1] += lastYOffset / 5;
				//print previous position and rotation info
				printf("postion = [%f, %f,%f]\n", navigationPosition[0], navigationPosition[1], navigationPosition[2]);
				printf("rotation = [%f,%f]\n", navigationRotation[0], navigationRotation[1]);
				break;
			}
			case SDL_BUTTON_RIGHT:	 
			{
				rightMouseButtonActive = 0;
				printf("Right Button is released\n");
				//update previous z-axis position
				navigationPosition[2] += lastXOffset / 10;
				//print previous position and rotation info
				printf("postion = [%f, %f,%f]\n", navigationPosition[0], navigationPosition[1], navigationPosition[2]);
				printf("rotation = [%f,%f]\n", navigationRotation[0], navigationRotation[1]);
				break;
			}
			case SDL_BUTTON_MIDDLE:	
			{
				middleMouseButtonActive = 0;
				printf("Middle Button is released\n");
				//update previous x,y-axis position
				navigationPosition[0] += lastXOffset / 20;
				navigationPosition[1] -= lastYOffset / 20;
				//print previous position and rotation info
				printf("postion = [%f, %f,%f]\n", navigationPosition[0], navigationPosition[1], navigationPosition[2]);
				printf("rotation = [%f,%f]\n", navigationRotation[0], navigationRotation[1]);
				break;
			}
			}
		}

		// Handle Mouse Motion Events
		if (evt.type == SDL_MOUSEMOTION) {
			if (leftMouseButtonActive || middleMouseButtonActive || rightMouseButtonActive)
			{
				//update x,y variation
				lastXOffset = evt.motion.x - mousePressedX;
				lastYOffset = evt.motion.y - mousePressedY;
			}
			else
			{
				//reset
				lastXOffset = 0;
				lastYOffset = 0;
			}
		}

		// ****************************************************************************
		// ****************************************************************************
		// ****************************************************************************
	}
	return true;
}

