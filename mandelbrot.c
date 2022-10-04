#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>

int main(void)
{
	if(!glfwInit())
	{
		fprintf(stderr, "Cannot initialize glfw!\n");
		return -1;
	}

	GLFWwindow* win = glfwCreateWindow(640, 640, "GPU Mandelbrot Set", NULL, NULL);
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		return -1;
	}
	glfwMakeContextCurrent(win);

	while(!glfwWindowShouldClose(win))
	{
		//glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	
	glfwTerminate();
}
