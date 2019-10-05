#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm\gtc\type_ptr.hpp> 
#include <glm\gtc\matrix_transform.hpp>
#include "Utils.h"

#define numVAOs 1
#define numVBOs 2

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
float x = 0.0f;		//location of triangle on x-axis
float inc = 0.01f;	//offset for moving the triangle
float cameraX, cameraY, cameraZ; 
float cubeLocX, cubeLocY, cubeLocZ;

// allocate variables used in display() function, so that they won’t need to be allocated during rendering 
GLuint mvLoc, projLoc; 
int width, height; 
float aspect; 
glm::mat4 pMat, vMat, mMat, mvMat;

using namespace std;

void setupVertices(void)
{ // 36 vertices, 12 triangles, makes 2x2x2 cube placed at origin 
	float vertexPositions[108] = {
									-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
									1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
									1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
									1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
									1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
									-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
									-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
									-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
									-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
									1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
									-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
									1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}



void init(GLFWwindow* window) 
{
	
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	
	//Camera setup
	cameraX = 0.0f; 
	cameraY = 0.0f; 
	cameraZ = 8.0f;

	cubeLocX = 0.0f;
	cubeLocY = -2.0f; // shift down Y to reveal perspective
	cubeLocZ = 0.0f; 

	setupVertices();

	
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	//Clear the background to black each time

	glUseProgram(renderingProgram);
	
	//Get the uniform variables for the mv and projection matrices
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	//Build the perspective matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = float(width) / float(height);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
	
	//Build the view matrix, model matrix and model-view matrix
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	mvMat = vMat * mMat;

	//Copy perspective and MV matrices to corresponding uniform variables
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//Associate VBO with the corresponding vertex attribute in the vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Adjust OpenGL settings and draw the model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}
int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter4 - program1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	init(window);
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


//Add code