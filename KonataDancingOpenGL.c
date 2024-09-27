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
#define NUM_TEXTURES 711

typedef struct
{
	double endTime, frameTime, waitTime, startTime;
} TimeManager;

static bool isDragging = false;
static unsigned char* imageData = NULL;
static POINT lastPos;

static const float vertex[] = { -1,-1,0,1,-1,0,1,1,0,-1,1,0 };
static const float texCord[] = { 0,1, 1,1, 1,0, 0,0 };

#define TARGET_FRAME_TIME 1.0f / 35.0f

static GLuint texturePool[NUM_TEXTURES];
static int currentTextureLoad = NUM_TEXTURES;

GLFWwindow* winInit();
void winHints();
void glfwErrLog();
void mainLoop(GLFWwindow* window, TimeManager* timeManager);
void render(const float* vertex, const float* texCord);
void loadImg(const int index);
void loadTexture(const int index);
void updataImgIndex(int* index);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
bool isOpaquePixel(const int x, const int y);
void setupRenderingState();
void fpsSync(TimeManager* timeManager);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!glfwInit())
		glfwErrLog();
	winHints();
	GLFWwindow* window = winInit();
	if (window == NULL)
		glfwErrLog();
	glfwSwapInterval(1);
	glewExperimental = true; // Core OpenGL
	if (glewInit() != GLEW_OK)
		glfwErrLog();

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	setupRenderingState();

	TimeManager* timeManager = malloc(sizeof(TimeManager));

	mainLoop(window, timeManager);
	return 0;
}

void mainLoop(GLFWwindow* window, TimeManager* timeManager)
{
	int imgIndex = 1;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(window))
	{
		timeManager->startTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);
		loadTexture(imgIndex);
		render(vertex, texCord);
		updataImgIndex(&imgIndex);
		glfwSwapBuffers(window);
		glfwPollEvents();
		if (currentTextureLoad > 0)
			stbi_image_free(imageData);
		fpsSync(timeManager);
	}
}

GLFWwindow* winInit()
{
	GLFWwindow* window = glfwCreateWindow(WIN_SIZE_W, WIN_SIZE_H, "", NULL, NULL);

	glfwMakeContextCurrent(window);

	HWND hwnd = glfwGetWin32Window(window);

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

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

void glfwErrLog()
{
#ifdef DEBUG
	const char* description;
	int code = glfwGetError(&description);
	fprintf(stderr, "ERROR! %d: %s\n", code, description);
#endif // DEBUG
	glfwTerminate();
}

void render(const float* vertex, const float* texCord)
{
	glPushMatrix();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// start render 
		glVertexPointer(3, GL_FLOAT, 0, vertex);
		glTexCoordPointer(2, GL_FLOAT, 0, texCord);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		// end render
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}

void loadTexture(const int index)
{
	if (currentTextureLoad > 0)
	{
		loadImg(index);

		glGenTextures(1, &texturePool[index]);
		glBindTexture(GL_TEXTURE_2D, texturePool[index]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_SIZE_W, WIN_SIZE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

		currentTextureLoad -= 2;
	}
	else glBindTexture(GL_TEXTURE_2D, texturePool[index]);
}

void loadImg(const int index)
{
	char path[32] = { '\0' };
	sprintf_s(path, 32, "frames\\frame_%04d.png", index);
	int dummy;
	imageData = stbi_load(path, &dummy, &dummy, &dummy, 0);
}

void updataImgIndex(int* index)
{
	if (*index == NUM_TEXTURES)
		*index = -1;
	*index += 2;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		HWND hwnd = glfwGetWin32Window(window);

		switch (action)
		{
			case GLFW_PRESS:
			{
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				int pixelX = (int)xpos;
				int pixelY = WIN_SIZE_H - (int)ypos;

				if (isOpaquePixel(pixelX, pixelY))
				{
					isDragging = true;
					GetCursorPos(&lastPos);
				}
				break;
			}
			case GLFW_RELEASE:
			{
				isDragging = false;
				break;
			}
		}
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

bool isOpaquePixel(const int x, const int y)
{
	unsigned char pixel[4];
	glReadPixels(x, WIN_SIZE_H - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	return pixel[3] > 0;
}

void setupRenderingState()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void fpsSync(TimeManager* timeManager)
{
	timeManager->endTime = glfwGetTime();
	timeManager->frameTime = timeManager->endTime - timeManager->startTime;

	if (timeManager->frameTime < TARGET_FRAME_TIME)
	{
		timeManager->waitTime = TARGET_FRAME_TIME - timeManager->frameTime;
		Sleep((DWORD)(timeManager->waitTime * 1000));
	}
}