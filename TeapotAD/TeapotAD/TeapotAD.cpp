// TeapotAD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "gl_core_4_3.hpp"

#include <glfw3.h>

#include "scene.h"

#include "scenediffuse.h"


//#include <string>
//using std::cout;


#include "QuatCamera.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define MOVE_VELOCITY 0.01f
#define ROTATE_VELOCITY 0.001f

using namespace imat2908;

//The GLFW Window
GLFWwindow *window;

//The Scene
Scene *scene;

//The camera
QuatCamera camera;

//To keep track of cursor location
double lastCursorPositionX, lastCursorPositionY, cursorPositionX, cursorPositionY;

/////////////////////////////////////////////////////////////////////////////////////////////
//Callback function for keypress use to toggle animate (not used at the moment)
// and to check for R to reset camera
/////////////////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow* window, int key, int cancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		if (scene)
			scene->animate(!(scene->animating()));
	if (key == 'R' && action == GLFW_RELEASE)
			camera.reset();
	// Toggle between the spotlight and world light. DOES NOT WORK //
	if (key == 'T' && action == GLFW_RELEASE) 
	{
		std::cout << camera.toggle() <<  std::endl;
		camera.changetoggle();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Callback function when mouse wheel is scrolled
/////////////////////////////////////////////////////////////////////////////////////////////
void scroll_callback(GLFWwindow *window, double x, double y)
{
			camera.zoom((float)y*0.5f);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Initialise 
/////////////////////////////////////////////////////////////////////////////////////////////
void initializeGL() {


    gl::ClearColor(0.5f,0.5f,0.5f,1.0f);
    
	lastCursorPositionX=0.0;
	lastCursorPositionY=0.0;
	cursorPositionX=0.0;
	cursorPositionY=0.0;
	

	// Create the scene class and initialise it for the camera
	scene = new SceneDiffuse();
    scene->initScene(camera);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Update
/////////////////////////////////////////////////////////////////////////////////////////////
void update( float t ) 
{ 
	
	//Get the current cursor position
	glfwGetCursorPos(window,&cursorPositionX,&cursorPositionY);

	//See how much the cursor has moved
	float deltaX = (float)(lastCursorPositionX - cursorPositionX);
	float deltaY = (float)(lastCursorPositionY - cursorPositionY);

	//Using a different way (i.e. instead of callback) to check for LEFT mouse button
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) )
	{
		//Rotate the camera. The 0.001f is a velocity mofifier to make the speed sensible

		camera.rotate(deltaX*ROTATE_VELOCITY, deltaY*ROTATE_VELOCITY);
		
	}
	
	//Using a different way (i.e. instead of callback) to check for RIGHT mouse button
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) )
	{
		//Rotate the camera. The 0.01f is a velocity mofifier to make the speed sensible
		camera.pan(deltaX*MOVE_VELOCITY, deltaY*MOVE_VELOCITY);

	}
	//To adjust Roll with MIDDLE mouse button
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE) )
	{
		// Perform the Roll method from QuatCamera.
		camera.roll(deltaX*ROTATE_VELOCITY);	
	}
		
	//Store the current cursor position
	lastCursorPositionX = cursorPositionX;
	lastCursorPositionY = cursorPositionY;
	
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Main loop updates scene and renders until we quit
/////////////////////////////////////////////////////////////////////////////////////////////
void mainLoop() {
	while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
		//GLUtils::checkForOpenGLError(__FILE__,__LINE__);
		update((float)glfwGetTime());
		scene->render(camera);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize
/////////////////////////////////////////////////////////////////////////////////////////////
void resizeGL(QuatCamera camera, int w, int h ) {
    scene->resize(camera,w,h);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// main entry point
/////////////////////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	string name ="Teapot with Phong Lighting";

	// Initialize GLFW
	if( !glfwInit() ) exit( EXIT_FAILURE );

	// Select OpenGL 4.3 with a forward compatible core profile.
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);

	// Open the window
	string title = "imat2908 - " + name;
	window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, title.c_str(), NULL, NULL );
	if( ! window ) {
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
	glfwMakeContextCurrent(window);

	//Key callback
	glfwSetKeyCallback(window,key_callback);

	//Mouse callback, not used at the moment
	//glfwSetMouseButtonCallback(window,mouse_callback);

	//Scroll callback
	glfwSetScrollCallback(window,scroll_callback);//Set callback

	// Load the OpenGL functions.
	gl::exts::LoadTest didLoad = gl::sys::LoadFunctions();

	if (!didLoad) {

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Initialization
	initializeGL();

	resizeGL(camera,WIN_WIDTH,WIN_HEIGHT);

	// Enter the main loop
	mainLoop();

	// Close window and terminate GLFW
	glfwTerminate();

	delete scene;

	// Exit program
	exit( EXIT_SUCCESS );
}




