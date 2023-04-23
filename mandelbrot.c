#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <math.h>

static unsigned int programId;
static unsigned int vertId;
static unsigned int fragId;

static int leftPressed = 0;

//Uniform locations
static int zoomUniformLocation;
static int centerUniformLocation;
static int iterationsUniformLocation;
static int startNumLocation;

static double zoomFactor = 1.0f;

#define START_NUM_CHANGE 0.003
static double startNum[] = { 0.0, 0.0 };

#define WINDOW_DEFAULT_WIDTH 1280.0f
#define WINDOW_DEFAULT_HEIGHT 640.0f
#define DEFAULT_ASPECT 640.0f / 1280.0f

#define ZOOM_AMOUNT 1.03

void handleWinResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	glUniform2d(zoomUniformLocation, 0.5f * (double)(newHeight) / (double)(newWidth) * 1.0f / (DEFAULT_ASPECT) * zoomFactor, 1.0f * zoomFactor);
}

void handleMouseScroll(GLFWwindow *win, double xoff, double yoff)
{
	if(yoff <= 0.0)
		zoomFactor *= 1.0 / ZOOM_AMOUNT;
	else if(yoff >= 0.0)
		zoomFactor *= ZOOM_AMOUNT;

	//Get window dimensions
	int height, width;
	glfwGetWindowSize(win, &width, &height); 

	glUniform2d(zoomUniformLocation, 0.5 * (double)(height) / (double)(width) * 1.0 / (DEFAULT_ASPECT) * zoomFactor, 1.0 * zoomFactor);
	glUniform1i(iterationsUniformLocation, (1.0 + log2(zoomFactor) / 64.0) * 128.0);
}

void handleMouseInput(GLFWwindow *win, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) leftPressed = 1;
	if(button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) leftPressed = 0;	
}

void handleKeyInput(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	switch(key)
	{
	case GLFW_KEY_UP:
		startNum[1] += START_NUM_CHANGE;
		break;
	case GLFW_KEY_DOWN:
		startNum[1] -= START_NUM_CHANGE;
		break;
	case GLFW_KEY_RIGHT:
		startNum[0] += START_NUM_CHANGE;
		break;
	case GLFW_KEY_LEFT:
		startNum[0] -= START_NUM_CHANGE;
		break;
	}
	glUniform2d(startNumLocation, startNum[0], startNum[1]);
}

void printShaderErrors(unsigned int shader)
{	
	int compiled, type;	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(shader, GL_SHADER_TYPE, &type);

	//Did not compile!
	if(!compiled)
	{
		switch(type)
		{
		case GL_VERTEX_SHADER: fprintf(stderr, "Vertex shader failed to compile!\n"); break;
		case GL_FRAGMENT_SHADER: fprintf(stderr, "Fragment shader failed to compile!\n"); break;
		default: fprintf(stderr, "Shader failed to compile!\n"); break;
		}

		//Get all the error messages
		int errLen, chWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLen);
		char* msg = (char*)malloc(errLen);
		glGetShaderInfoLog(shader, errLen, &chWritten, msg);
		fprintf(stderr, "%s", msg);

		free(msg);
	}
}

void setupShaderProgram()
{
	programId = glCreateProgram();
	vertId = glCreateShader(GL_VERTEX_SHADER);
	fragId = glCreateShader(GL_FRAGMENT_SHADER);

	//Open the shader files
	FILE* vert = fopen("mandelbrot-vert.glsl", "r");
	FILE* frag = fopen("mandelbrot-frag.glsl", "r");

	//Read the shader files
	char* vertSrc = (char*)malloc(4096);
	char* fragSrc = (char*)malloc(4096);

	int vertAtEOF = 0,
		fragAtEOF = 0;

	//Read the vertex shader file
	for(int i = 0; i < 4096; i++)
	{
		if(vertAtEOF)
		{
			vertSrc[i] = ' ';
			continue;	
		}	

		vertSrc[i] = fgetc(vert);
		if(vertSrc[i] == EOF)
		{
			vertSrc[i] = ' ';
			vertAtEOF = 1;	
		}	
	}
	vertSrc[4095] = '\0';

	//Read the fragment shader file
	for(int i = 0; i < 4096; i++)
	{
		if(fragAtEOF)
		{
			fragSrc[i] = ' ';
			continue;	
		}	

		fragSrc[i] = fgetc(frag);
		if(fragSrc[i] == EOF)
		{
			fragSrc[i] = ' ';
			fragAtEOF = 1;	
		}	
	}
	fragSrc[4095] = '\0';

	//Send source code to shaders
	const char* vertBegin = vertSrc;
	const char* fragBegin = fragSrc;	
	glShaderSource(vertId, 1, &vertBegin, NULL); 
	glShaderSource(fragId, 1, &fragBegin, NULL);
	glCompileShader(vertId);
	glCompileShader(fragId);
	
	printShaderErrors(vertId);
	printShaderErrors(fragId);

	glAttachShader(programId, vertId);
	glAttachShader(programId, fragId);	
	glLinkProgram(programId);
	glValidateProgram(programId);
	
	glUseProgram(programId);

	glDeleteShader(vertId);
	glDeleteShader(fragId);	

	//Get uniforms
	zoomUniformLocation = glGetUniformLocation(programId, "uScale");
	centerUniformLocation = glGetUniformLocation(programId, "uCenter");
	iterationsUniformLocation = glGetUniformLocation(programId, "uDepth");
	startNumLocation = glGetUniformLocation(programId, "uStartNum");

	//Close files and free memory
	free(vertSrc);
	free(fragSrc);
	fclose(vert);
	fclose(frag);
}

void outputErrors()
{
	GLenum err = glGetError();
	while(err != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL Error: %d\n", err);
		err = glGetError();	
	}
}

int main(void)
{
	if(!glfwInit())
	{
		fprintf(stderr, "Cannot initialize glfw!\n");
		return -1;
	}

	GLFWwindow* win = glfwCreateWindow(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "GPU Mandelbrot Set", NULL, NULL);
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		return -1;
	}
	glfwMakeContextCurrent(win);

	//Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		fprintf(stderr, "Failed to load glad!\n");	
		return -1;
	}

	//Create a rectangle buffer object
	const double rect[] = 
	{
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f,

		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	unsigned int buffId;
	glGenBuffers(1, &buffId);
	glBindBuffer(GL_ARRAY_BUFFER, buffId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(double) * 2, NULL);
	glEnableVertexAttribArray(0);

	setupShaderProgram();
	glfwSetWindowSizeCallback(win, handleWinResize);
	glfwSetScrollCallback(win, handleMouseScroll);
	glfwSetMouseButtonCallback(win, handleMouseInput);
	glfwSetKeyCallback(win, handleKeyInput);

	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(win);
		glfwPollEvents();

		outputErrors();

		static double centerX = 0.0f, centerY = 0.0f;	
		static double cursorX = -1.0, cursorY = -1.0;

		//Move center around
		if(leftPressed)
		{
			double tempX, tempY;
			glfwGetCursorPos(win, &tempX, &tempY);
	
			if(cursorX == -1.0 || cursorY == -1.0)
			{
				cursorX = tempX;
				cursorY = tempY;
				continue;
			}
	
			//Get window dimensions
			int width, height;
			glfwGetWindowSize(win, &width, &height);
	
			//Get difference in mouse position
			double xDiff = (tempX - cursorX) / (double)width;
			double yDiff = (cursorY - tempY) / (double)height;
	
			centerX -= xDiff * 2.0f / zoomFactor;
			centerY -= yDiff * 2.0f / zoomFactor;
	
			glUniform2d(centerUniformLocation, centerX, centerY);
	
			cursorX = tempX;
			cursorY = tempY;
		}
		else
		{
			cursorX = -1.0;
			cursorY = -1.0;
		}

		//fprintf(stderr, "Zoom: %f\n", zoomFactor);
	}
	
	glfwTerminate();
}
