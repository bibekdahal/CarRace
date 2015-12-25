#include "../utils/stdinc.h"
#include "Window.h"

std::function<void(int, int)> Window::ms_resize = [](int, int){};

Window::Window() : m_update([](double, double){}), m_render([](){}), m_timer(60.0)
{}

void Window::CreateWindow(std::string title, int &width, int &height, bool fullscreen)
{
    if (!glfwInit()) throw WindowException("Failed tp initialize GLFW", WindowException::FAIL_GLFW_INIT);

    glfwWindowHint(GLFW_SAMPLES, 4);
    m_window = glfwCreateWindow(width, height, title.c_str() , fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        throw WindowException("Failed to create window", WindowException::FAIL_WINDOW_CREATE);
    }

    glfwMakeContextCurrent(m_window);

    if (glewInit() != GLEW_OK)
        throw WindowException("Failed to initialize GLEW", WindowException::FAIL_GLEW_INIT);

    glfwGetFramebufferSize(m_window, &width, &height);
    glfwSetFramebufferSizeCallback(m_window, Resize);

    m_timer.Reset(240.0);
}

void Window::MainLoop()
{
    while (!glfwWindowShouldClose(m_window) && glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        
        char dest[20] = "FPS: ";  _itoa_s(m_timer.GetFPS(), dest + 5, 20 - 5, 10);
        glfwSetWindowTitle(m_window, dest);
        //
        m_timer.Update(m_update);
        m_render();
        SwapBuffers();
        glfwPollEvents();
    }
}

void Window::Destroy()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}