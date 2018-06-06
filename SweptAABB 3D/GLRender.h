#ifndef _GL_RENDER_H
#define _GL_RENDER_H

#include "GLIncludes.h"
#include "GameObject.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// This is your reference to your shader program.
// This will be assigned with glCreateProgram().
// This program will run on your GPU.
GLuint program;

// These are your references to your actual compiled shaders
GLuint vertex_shader;
GLuint fragment_shader;

// This is a reference to your uniform MVP matrix in your vertex shader
GLuint uniMVP;

// These are 4x4 transformation matrices, which you will locally modify before passing into the vertex shader via uniMVP
glm::mat4 proj;
glm::mat4 view;

// proj * view = PV
glm::mat4 PV;

// MVP is PV * Model (model is the transformation matrix of whatever object is being rendered)
glm::mat4 MVP;
glm::mat4 MVP2;

// Variable for the speed of the moving object.
float speed = 0.90f;

// An array of vertices stored in an std::vector for our object.
std::vector<VertexFormat> vertices;

// References to our two GameObjects and the one Model we'll be using.
GameObject* obj1;
GameObject* obj2;
Model* cube;

// This function runs every frame
void renderScene()
{
	// Clear the color buffer and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Clear the screen to white
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Tell OpenGL to use the shader program you've created.
	glUseProgram(program);

	// Set the uniform matrix in our shader to our MVP matrix for the first object.
	glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVP));

	// Draw the cube.
	cube->Draw();

	// Set the uniform matrix in our shader to our MVP matrix for the second object.
	glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVP2));

	// Draw the cube again.
	cube->Draw();

	// We're using the same model here to draw, but different transformation matrices so that we can use less data overall.
	// This is a technique called instancing, although "true" instancing involves binding a matrix array to the uniform variable and using DrawInstanced in place of draw.
}

// This method reads the text from a file.
// Realistically, we wouldn't want plain text shaders hardcoded in, we'd rather read them in from a separate file so that the shader code is separated.
std::string readShader(std::string fileName)
{
	std::string shaderCode;
	std::string line;

	// We choose ifstream and std::ios::in because we are opening the file for input into our program.
	// If we were writing to the file, we would use ofstream and std::ios::out.
	std::ifstream file(fileName, std::ios::in);

	// This checks to make sure that we didn't encounter any errors when getting the file.
	if (!file.good())
	{
		std::cout << "Can't read file: " << fileName.data() << std::endl;

		// Return so we don't error out.
		return "";
	}

	// ifstream keeps an internal "get" position determining the location of the element to be read next
	// seekg allows you to modify this location, and tellg allows you to get this location
	// This location is stored as a streampos member type, and the parameters passed in must be of this type as well
	// seekg parameters are (offset, direction) or you can just use an absolute (position).
	// The offset parameter is of the type streamoff, and the direction is of the type seekdir (an enum which can be ios::beg, ios::cur, or ios::end referring to the beginning, 
	// current position, or end of the stream).
	file.seekg(0, std::ios::end);					// Moves the "get" position to the end of the file.
	shaderCode.resize((unsigned int)file.tellg());	// Resizes the shaderCode string to the size of the file being read, given that tellg will give the current "get" which is at the end of the file.
	file.seekg(0, std::ios::beg);					// Moves the "get" position to the start of the file.

													// File streams contain two member functions for reading and writing binary data (read, write). The read function belongs to ifstream, and the write function belongs to ofstream.
													// The parameters are (memoryBlock, size) where memoryBlock is of type char* and represents the address of an array of bytes are to be read from/written to.
													// The size parameter is an integer that determines the number of characters to be read/written from/to the memory block.
	file.read(&shaderCode[0], shaderCode.size());	// Reads from the file (starting at the "get" position which is currently at the start of the file) and writes that data to the beginning
													// of the shaderCode variable, up until the full size of shaderCode. This is done with binary data, which is why we must ensure that the sizes are all correct.

	file.close(); // Now that we're done, close the file and return the shaderCode.

	return shaderCode;
}

// This method will consolidate some of the shader code we've written to return a GLuint to the compiled shader.
// It only requires the shader source code and the shader type.
GLuint createShader(std::string sourceCode, GLenum shaderType)
{
	// glCreateShader, creates a shader given a type (such as GL_VERTEX_SHADER) and returns a GLuint reference to that shader.
	GLuint shader = glCreateShader(shaderType);
	const char *shader_code_ptr = sourceCode.c_str(); // We establish a pointer to our shader code string
	const int shader_code_size = sourceCode.size();   // And we get the size of that string.

													  // glShaderSource replaces the source code in a shader object
													  // It takes the reference to the shader (a GLuint), a count of the number of elements in the string array (in case you're passing in multiple strings), a pointer to the string array 
													  // that contains your source code, and a size variable determining the length of the array.
	glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shader); // This just compiles the shader, given the source code.

	GLint isCompiled = 0;

	// Check the compile status to see if the shader compiled correctly.
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infolog);

		// Print the compile error.
		std::cout << "The shader failed to compile with the error:" << std::endl << infolog << std::endl;

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.

								// NOTE: I almost always put a break point here, so that instead of the program continuing with a deleted/failed shader, it stops and gives me a chance to look at what may 
								// have gone wrong. You can check the console output to see what the error was, and usually that will point you in the right direction.
	}

	return shader;
}

void setupCube()
{
	// An element array, which determines which of the vertices to display in what order. This is sometimes known as an index array.
	GLuint elements[] = {
		0, 1, 2, 0, 2, 3, 3, 2, 4, 3, 4, 5, 5, 4, 6, 5, 6, 7, 7, 6, 1, 7, 1, 0, 1, 6, 4, 1, 4, 2, 7, 0, 3, 7, 3, 5
	};
	// These are the indices for a cube.
	vertices.push_back(VertexFormat(glm::vec3(-0.25, -0.25, 0.25),		// Front, Bottom, Left		0
		glm::vec4(1.0, 0.0, 0.0, 1.0))); //red
	vertices.push_back(VertexFormat(glm::vec3(-0.25, 0.25, 0.25),		// Front, Top, Left			1
		glm::vec4(1.0, 0.0, 0.0, 1.0))); //red
	vertices.push_back(VertexFormat(glm::vec3(0.25, 0.25, 0.25),		// Front, Top, Right		2
		glm::vec4(1.0, 0.0, 1.0, 1.0))); //yellow
	vertices.push_back(VertexFormat(glm::vec3(0.25, -0.25, 0.25),		// Front, Bottom, Right		3
		glm::vec4(1.0, 0.0, 1.0, 1.0))); //yellow
	vertices.push_back(VertexFormat(glm::vec3(0.25, 0.25, -0.25),		// Back, Top, Right			4
		glm::vec4(0.0, 1.0, 1.0, 1.0))); //cyan
	vertices.push_back(VertexFormat(glm::vec3(0.25, -0.25, -0.25),		// Back, Bottom, Right		5
		glm::vec4(0.0, 1.0, 1.0, 1.0))); //cyan
	vertices.push_back(VertexFormat(glm::vec3(-0.25, 0.25, -0.25),		// Back, Top, Left			6
		glm::vec4(0.0, 1.0, 0.0, 1.0))); //blue
	vertices.push_back(VertexFormat(glm::vec3(-0.25, -0.25, -0.25),		// Back, Bottom, Left		7
		glm::vec4(0.0, 1.0, 0.0, 1.0))); //blue

										 // Create our cube model from the calculated data.
	cube = new Model(vertices.size(), vertices.data(), 36, elements);

	// Create two GameObjects based off of the cube model (note that they are both holding pointers to the cube, not actual copies of the cube vertex data).
	obj1 = new GameObject(cube);
	obj2 = new GameObject(cube);

	// Set beginning properties of GameObjects.
	obj1->SetVelocity(glm::vec3(0, 0.0f, 0.0f)); // The first object doesn't move.
	obj2->SetVelocity(glm::vec3(-speed, -speed, -speed));
	obj1->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	obj2->SetPosition(glm::vec3(0.7f, 0.7f, 0.7f));
	obj1->SetScale(glm::vec3(0.75f, 0.75f, 0.75f));
	obj2->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
}

// Initialization code
void init()
{
	// Initializes the glew library
	glewInit();

	// Enables the depth test, which you will want in most cases. You can disable this in the render loop if you need to.
	glEnable(GL_DEPTH_TEST);

	setupCube();

	// Read in the shader code from a file.
	std::string vertShader = readShader("../VertexShader.glsl");
	std::string fragShader = readShader("../FragmentShader.glsl");

	// createShader consolidates all of the shader compilation code
	vertex_shader = createShader(vertShader, GL_VERTEX_SHADER);
	fragment_shader = createShader(fragShader, GL_FRAGMENT_SHADER);

	// A shader is a program that runs on your GPU instead of your CPU. In this sense, OpenGL refers to your groups of shaders as "programs".
	// Using glCreateProgram creates a shader program and returns a GLuint reference to it.
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);		// This attaches our vertex shader to our program.
	glAttachShader(program, fragment_shader);	// This attaches our fragment shader to our program.

												// This links the program, using the vertex and fragment shaders to create executables to run on the GPU.
	glLinkProgram(program);
	// End of shader and program creation

	// This gets us a reference to the uniform variable in the vertex shader, which is called "MVP".
	// We're using this variable as a 4x4 transformation matrix
	// Only 2 parameters required: A reference to the shader program and the name of the uniform variable within the shader code.
	uniMVP = glGetUniformLocation(program, "MVP");

	// Creates the view matrix using glm::lookAt.
	// First parameter is camera position, second parameter is point to be centered on-screen, and the third paramter is the up axis.
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Creates a projection matrix using glm::perspective.
	// First parameter is the vertical FoV (Field of View), second paramter is the aspect ratio, 3rd parameter is the near clipping plane, 4th parameter is the far clipping plane.
	proj = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	// Allows us to make one less calculation per frame, as long as we don't update the projection and view matrices every frame.
	PV = proj * view;

	// Create your MVP matrices based on the objects' transforms.
	MVP = PV * *obj1->GetTransform();
	MVP2 = PV * *obj2->GetTransform();

	

	// This is not necessary, but I prefer to handle my vertices in the clockwise order. glFrontFace defines which face of the triangles you're drawing is the front.
	// Essentially, if you draw your vertices in counter-clockwise order, by default (in OpenGL) the front face will be facing you/the screen. If you draw them clockwise, the front face 
	// will face away from you. By passing in GL_CW to this function, we are saying the opposite, and now the front face will face you if you draw in the clockwise order.
	// If you don't use this, just reverse the order of the vertices in your array when you define them so that you draw the points in a counter-clockwise order.
	glFrontFace(GL_CW);

	// This is also not necessary, but more efficient and is generally good practice. By default, OpenGL will render both sides of a triangle that you draw. By enabling GL_CULL_FACE, 
	// we are telling OpenGL to only render the front face. This means that if you rotated the triangle over the X-axis, you wouldn't see the other side of the triangle as it rotated.
	glEnable(GL_CULL_FACE);

	// Determines the interpretation of polygons for rasterization. The first parameter, face, determines which polygons the mode applies to.
	// The face can be either GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
	// The mode determines how the polygons will be rasterized. GL_POINT will draw points at each vertex, GL_LINE will draw lines between the vertices, and 
	// GL_FILL will fill the area inside those lines.
	glPolygonMode(GL_FRONT, GL_FILL);
}

void cleanup()
{
	// After the program is over, cleanup your data!
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
	// Note: If at any point you stop using a "program" or shaders, you should free the data up then and there.

	delete(obj1);
	delete(obj2);
	delete(cube);

	// Frees up GLFW memory
	glfwTerminate();
}

#endif _GL_RENDER_H