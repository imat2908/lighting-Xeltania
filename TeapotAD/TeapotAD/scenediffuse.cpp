
#include "scenediffuse.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include "defines.h"

using glm::vec3;


#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>


namespace imat2908
{
/////////////////////////////////////////////////////////////////////////////////////////////
// Default constructor
/////////////////////////////////////////////////////////////////////////////////////////////
SceneDiffuse::SceneDiffuse()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Initialise the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::initScene(QuatCamera camera)
{
    //|Compile and link the shader  
	compileAndLinkShader();

    gl::Enable(gl::DEPTH_TEST);

 	//Set up the lighting
	setLightParams(camera);

	//Create the plane to represent the ground
	plane = new VBOPlane(100.0,100.0,100,100);

	//A matrix to move the teapot lid upwards
	glm::mat4 lid = glm::mat4(1.0);
	//lid *= glm::translate(vec3(0.0,1.0,0.0));

	//Create the teapot with translated lid
	teapot = new VBOTeapot(16,lid);

}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update not used at present
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::update(float t)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::setLightParams(QuatCamera camera)
{

	vec3 worldLight = vec3(10.0f, 10.0f,10.0f);

			// World Light Params	 //
	// Diffuse
	prog.setUniform("ADS.Ld", 0.9f, 0.9f, 0.9f);//what elements does Ld have?
	// Ambient
	prog.setUniform("ADS.La", 0.3f, 0.3f, 0.3f);
	//Specular
	prog.setUniform("ADS.Ls", 0.9f, 0.9f, 0.9f);
	

			//	Spotlight Params	//
	prog.setUniform("Spotlight.pos", 0, 0.5, -20);
	prog.setUniform("Spotlight.direction", 0, -1, 0.f); 
	prog.setUniform("Spotlight.cutoff", glm::cos(glm::radians(0.15f)));
	// ADS //
	prog.setUniform("Spotlight.La", 0.1,0.1,0.1 );
	prog.setUniform("Spotlight.Ld", 0.8,0.8,0.8);
	prog.setUniform("Spotlight.Ls", 1,1,1 );

	prog.setUniform("LightPosition", worldLight);
	prog.setUniform("Cam.View", camera.view());
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::render(QuatCamera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	//First deal with the plane to represent the ground

	//Initialise the model matrix for the plane
	model = mat4(1.0f);
	//Set the matrices for the plane although it is only the model matrix that changes so could be made more efficient
    setMatrices(camera);

	//Set the plane's material properties in the shader and render
	// Diffuse
	prog.setUniform("ADS.Kd", 0.51f, 1.0f, 0.49f); // What elements does Kd have?
	// Ambient
	prog.setUniform("ADS.Ka", 0.51f, 1.0f, 0.49f);
	// Specular
	prog.setUniform("ADS.Ks", 0.1f, 0.1f, 0.1f);

	// Spotlight Properties
	prog.setUniform("Spotlight.Ka", 0.4, 0.4, 0.4);
	prog.setUniform("Spotlight.Kd", 0.2, 0.5, 0.2);
	prog.setUniform("Spotlight.Ks", 0.3, 0.3, 0.3);

	plane->render();

	//Now set up the teapot 
	 model = mat4(1.0f);
	 setMatrices(camera);

	 //Set the Teapot material properties in the shader and render
	 // Diffuse
	 prog.setUniform("ADS.Kd", 0.46f, 0.29f, 0.0f);
	 // Ambient
	 prog.setUniform("ADS.Ka", 0.46f, 0.29f, 0.0f);
	 // Specular
	 prog.setUniform("ADS.Ks", 0.29f, 0.29f, 0.29f);

	 // Spotlight Properties
	 prog.setUniform("Spotlight.Ka", 0.2, 0.2, 0.2);
	 prog.setUniform("Spotlight.Kd", 0.9, 0.5, 0.3);
	 prog.setUniform("Spotlight.Ks", 0.95, 0.95, 0.95);

	 teapot->render(); // Render the teapot to the window.
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::setMatrices(QuatCamera camera)
{
    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", camera.projection() * mv);
	mat3 normMat = glm::transpose(glm::inverse(mat3(model)));// What does this line do?
	prog.setUniform("M", model);
	prog.setUniform("V", camera.view() );
	prog.setUniform("P", camera.projection() );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::resize(QuatCamera camera,int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
	camera.setAspectRatio((float)w/h);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::compileAndLinkShader()
{
	try 
	{
    	prog.compileShader("Shaders/phong.vert");
    	prog.compileShader("Shaders/phong.frag");
    	prog.link();
    	prog.validate();
    	prog.use();
    } 
	catch(GLSLProgramException & e) 
	{
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}
}
