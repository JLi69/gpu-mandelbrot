#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

void setupShaderProgram()
{
	static unsigned int programId;
	static unsigned int vertId;
	static unsigned int fragId;

	programId = glCreateProgram();
	vertId = glCreateShader(GL_VERTEX_SHADER);
	fragId = glCreateShader(GL_FRAGMENT_SHADER);

	//Open the shader files
	FILE* vert = fopen("mandelbrot-vert.glsl", "r");
	FILE* frag = fopen("mandelbrot-frag.glsl", "r");

	//Read the shader files
	char* vertSrc = (char*)malloc(8192);
	char* fragSrc = (char*)malloc(8192);

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
	glAttachShader(programId, vertId);
	glAttachShader(programId, fragId);	
	glLinkProgram(programId);
	glValidateProgram(programId);
	
	glUseProgram(programId);

	glDeleteShader(vertId);
	glDeleteShader(fragId);	

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

	GLFWwindow* win = glfwCreateWindow(1280, 640, "GPU Mandelbrot Set", NULL, NULL);
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
	float rect[] = 
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	glEnableVertexAttribArray(0);

	setupShaderProgram();

	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(win);
		glfwPollEvents();

		outputErrors();
	}
	
	glfwTerminate();
}
