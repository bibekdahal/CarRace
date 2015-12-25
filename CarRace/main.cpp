#include "utils/Libraries.h"
#include "graphics/Window.h"

#include "Game.h"

// Uncomment this to disable console window
// #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")


Window g_window;

int g_width = 800;// 1366;
int g_height = 600;// 768;
bool g_fullscreen = false;

Game g_game(&g_window);

void LoadConfig()
{
    using namespace luabridge;
    LuaScript m_script("scripts/config.lua");
    
    LuaRef ww = m_script.GetEnvironment()["WindowWidth"];
    LuaRef wh = m_script.GetEnvironment()["WindowHeight"];
    LuaRef fs = m_script.GetEnvironment()["Fullscreen"];
    if (ww.isNumber()) g_width = ww;
    if (wh.isNumber()) g_height = wh;
    if (!fs.isNil()) g_fullscreen = fs.cast<bool>();
}

void CleanUp()
{
    g_game.CleanUp();
    g_window.Close();
    g_window.Destroy();
    LuaScript::CleanUp();
}

void Fatal(std::string errorString)
{
    CleanUp();
    std::cout << "Program Terminated" << std::endl;
    std::cout << "ERROR: " << errorString << std::endl;
    std::cin.get();
}

int main()
{
    try
    {
        LuaScript::Init();
        LuaScript::AddEverything(&g_window);
        LoadConfig();
        g_window.CreateWindow("Car Race", g_width, g_height, g_fullscreen);
        g_window.ResizeFunc(std::bind(&Game::Resize, &g_game, std::placeholders::_1, std::placeholders::_2));
        g_window.RenderFunc(std::bind(&Game::Render, &g_game));
        g_window.UpdateFunc(std::bind(&Game::Update, &g_game, std::placeholders::_1, std::placeholders::_2));

        g_game.Initialize();
        g_game.Resize(g_width, g_height);
        g_window.MainLoop();
    }
    catch (Exception &e)
    {
        Fatal(e.GetErrorString());
        return 0;
    }
#ifndef _DEBUG
    catch (std::exception &e)
    {
        Fatal(e.what());
        return 0;
    }
#endif
    CleanUp();
    return 0;
}
