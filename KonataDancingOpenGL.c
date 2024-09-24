#define GLEW_STATIC
#include <GL/glew.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdbool.h>
#include <windows.h>

#define WIN_SIZE_H 360
#define WIN_SIZE_W 494

static bool isDragging = false;
static unsigned char* imageData = NULL;
static POINT lastPos;

static const float vertex[] = { -1,-1,0,1,-1,0,1,1,0,-1,1,0 };
static const float texCord[] = { 0,1, 1,1, 1,0, 0,0 };

#define TARGET_FRAME_TIME 1.0f / 35.0f

#define DEBUG

GLFWwindow* winInit();
void winHints();
void errLog();
void mainLoop();
void render();
void loadImg(int index);
void updataImgIndex(int* index);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
bool isOpaquePixel(int x, int y);
void setupRenderingState();

int WinMain()
{
	if (!glfwInit())
		errLog();
	winHints();
	GLFWwindow* window = winInit();
	if (window == NULL)
		errLog();
	glfwSwapInterval(1);
	glewExperimental = true; // Core OpenGL
	if (glewInit() != GLEW_OK)
		errLog();

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	setupRenderingState();
	mainLoop(window);
	return 0;
}

void mainLoop(GLFWwindow* window)
{
	const float vertex[] = { -1,-1,0,1,-1,0,1,1,0,-1,1,0 };
	const float texCord[] = { 0,1, 1,1, 1,0, 0,0 };

	int imgIndex = 1;

	GLuint texture;

	double endTime, frameTime, waitTime;

	while (!glfwWindowShouldClose(window))
	{
		double startTime = glfwGetTime();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		loadImg(imgIndex, &texture);
		render(texture, vertex, texCord);
		glDeleteTextures(1, &texture);
		updataImgIndex(&imgIndex);
		glfwSwapBuffers(window);
		glfwPollEvents();
		stbi_image_free(imageData);

		endTime = glfwGetTime();
		frameTime = endTime - startTime;

		if (frameTime < TARGET_FRAME_TIME)
		{
			waitTime = TARGET_FRAME_TIME - frameTime;
			while (glfwGetTime() - endTime < waitTime);
		}
	}
}

GLFWwindow* winInit()
{
	GLFWwindow* window;
	window = glfwCreateWindow(WIN_SIZE_W, WIN_SIZE_H, "", NULL, NULL);
	glfwMakeContextCurrent(window);

	HWND hwnd = glfwGetWin32Window(window);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return window;
}

void winHints()
{
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
}

void errLog()
{
	const char* description;
	int code = glfwGetError(&description);
	fprintf(stderr, "ERROR! %d: %s\n", code, description);
	glfwTerminate();
}

void render(GLuint texture, const float* vertex, const float* texCord)
{
	glBindTexture(GL_TEXTURE_2D, texture);

	glPushMatrix();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// render 
		glVertexPointer(3, GL_FLOAT, 0, vertex);
		glTexCoordPointer(2, GL_FLOAT, 0, texCord);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		// render
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}

void loadImg(int index, GLuint* texture)
{
	char path[32] = { '\0' };
	sprintf_s(path, 32, "frames\\frame_%04d.png", index);
	int width, height, cnt;
	imageData = stbi_load(path, &width, &height, &cnt, 0);

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
}

void updataImgIndex(int* index)
{
	*index += 2;
	if (*index == 713)
		*index = 1;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		HWND hwnd = glfwGetWin32Window(window);

		if (action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			int pixelX = (int)xpos;
			int pixelY = WIN_SIZE_H - (int)ypos;

			if (isOpaquePixel(pixelX, pixelY))
			{
				isDragging = true;

				POINT cursorPos;
				GetCursorPos(&cursorPos);
				lastPos = cursorPos;

				RECT rect;
				GetWindowRect(hwnd, &rect);
			}
		}
		else if (action == GLFW_RELEASE)
			isDragging = false;
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (isDragging)
	{
		HWND hwnd = glfwGetWin32Window(window);

		POINT cursorPos;
		GetCursorPos(&cursorPos);

		int dx = cursorPos.x - lastPos.x;
		int dy = cursorPos.y - lastPos.y;

		RECT rect;
		GetWindowRect(hwnd, &rect);
		SetWindowPos(hwnd, HWND_TOP, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

		lastPos = cursorPos;
	}
}

bool isOpaquePixel(int x, int y)
{
	if (!imageData) return false;

	int index = (y * WIN_SIZE_W + x) * 4;

	unsigned char alpha = imageData[index + 3];
	return alpha > 0;
}

void setupRenderingState()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}