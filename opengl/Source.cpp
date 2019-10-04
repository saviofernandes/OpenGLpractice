#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Utils.h"

#define numVAOs 1
GLuint renderingProgram;
GLuint vao[numVAOs];
float x = 0.0f;		//location of triangle on x-axis
float inc = 0.01f;	//offset for moving the triangle


using namespace std;

void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader info log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program info log: " << log << endl;
		free(log);
	}
}

bool checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}

	return foundError;
}

string readShaderSource(const char* filePath)
{
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

//Old create shader program code
GLuint createShaderProgram() {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource("vertShader.glsl");
	string fragShaderStr = readShaderSource("fragShader.glsl");

	const char* vshaderSource = vertShaderStr.c_str();
	const char* fshaderSource = fragShaderStr.c_str();
	
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		cout << "Linking failed" << endl;
		printProgramLog(vfProgram);
	}

	return vfProgram;
}





void init(GLFWwindow* window) 
{
	//renderingProgram = createShaderProgram();
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	//Clear the background to black each time

	glUseProgram(renderingProgram);
	//glPointSize(30.0f);
	
	x += inc;
	if (x > 1.0) inc = -0.01f;
	if (x < -1.0) inc = 0.01f;

	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
	glProgramUniform1f(renderingProgram, offsetLoc, x);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - program1", NULL, NULL);
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