#pragma once

#include "Timer.h"

class WindowException : public Exception {
public:
    enum WINDOWEXCEPTIONTYPE{ FAIL_GLFW_INIT, FAIL_WINDOW_CREATE, FAIL_GLEW_INIT };
    WindowException(std::string errorString, WINDOWEXCEPTIONTYPE errorType) : Exception(errorString, errorType) {}
};

class Window
{
public:

    Window();
    void CreateWindow(std::string title, int &width, int &height, bool fullscreen = false);
    void MainLoop();
    void Destroy();
    void SwapBuffers() { glfwSwapBuffers(m_window); }
    void Close() { glfwSetWindowShouldClose(m_window, GL_TRUE); }

    void ResizeFunc(std::function<void(int, int)> resize)		            { if (resize) ms_resize = resize; }
    void UpdateFunc(std::function<void(double, double)> update)	{ if (update) m_update = update; }
    void RenderFunc(std::function<void()> render)				{ if (render) m_render = render; }

    bool CheckKey(int key) { return glfwGetKey(m_window, key) == GLFW_PRESS; }
    bool CheckMButton(int button) { return glfwGetMouseButton(m_window, button) == GLFW_PRESS; }

    void GetCursorPos(double &x, double &y) { glfwGetCursorPos(m_window, &x, &y); }
    void SeCursorPos(double x, double y) { glfwSetCursorPos(m_window, x, y); }
    void ShowMouseCursor(bool show) { glfwSetInputMode(m_window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
    void DisableCursor() { glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

    void ResetTimer(double FPS = 60.0) { m_timer.Reset(FPS); }

    static void Resize(GLFWwindow * window, int width, int height) { ms_resize(width, height); }

private:
    static std::function<void(int, int)> ms_resize;
    std::function<void(double, double)> m_update;
    std::function<void()> m_render;

    Timer m_timer;
    GLFWwindow * m_window;
};
