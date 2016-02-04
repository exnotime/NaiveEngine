#include "Window.h"
#include <GLFW/glfw3.h>
using namespace core;
Window::Window() {

}

Window::~Window() {
	glfwDestroyWindow(m_Window);
	m_Window = nullptr;
}

void Window::Initialize(const WindowSettings& windowSettings) {
	m_WindowSettings = windowSettings;
	if (m_Window) {
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, m_WindowSettings.Resizeable ? true : false);
	glfwWindowHint(GLFW_DECORATED, m_WindowSettings.BorderLess ? false : true);
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	//glfwWindowHint(GLFW_SRGB_CAPABLE, true);
	//glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
	// Create an application window with the following settings:
	m_Window = glfwCreateWindow(m_WindowSettings.Width, m_WindowSettings.Height, m_WindowSettings.Title.c_str(), nullptr, nullptr);

	if (m_Window == nullptr) {
		printf("error creating window\n");
		return;
	}
	//glfwMakeContextCurrent(m_Window);
	//set up vsync
	if (m_WindowSettings.Vsync) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
}

GLFWwindow* Window::GetWindow() const {
	return m_Window;
}

const WindowSettings& Window::GetWindowSettings() const {
	return m_WindowSettings;
}

void Window::MakeCurrent() {
	glfwMakeContextCurrent(m_Window);
}
