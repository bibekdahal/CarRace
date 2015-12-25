#pragma once

#include "graphics/StandardMaterials.h"
#include "engine/Engine.h"
//#include "MyScene.h"
#include "lua/LuaScriptSystem.h"
#include "lua/LuaScene.h"


class GameException : public Exception {
public:
    enum GAMEXCEPTIONTYPE{ NO_SCENE };
    GameException(std::string errorString, int errorType) : Exception(errorString, errorType) {}
};

extern Engine* g_engine;
class Game
{
public:
    Game(Window* window) : m_window(window)/*, m_scene(window, &m_engine, &m_graphics)*/ {}
    void Initialize();

    void Resize(int widthI, int heightI)
    {
        float width = (float)widthI;
        float height = (float)heightI;
        m_graphics.Resize(width, height);
        m_engine.Resize(width, height);
    }
    void Update(double totalTime, double deltaTime)
    {
        m_engine.Update(deltaTime);
    }
    void Render()
    {
        m_graphics.BeginRender();
        for (m_graphics.FirstPass(); m_graphics.IsValidPass(); m_graphics.NextPass())
            m_engine.Render();
    }
    void CleanUp()
    {
        m_graphics.CleanUp();
        m_engine.CleanUp();
        //m_scene.CleanUp();
        for (unsigned i = 0; i < m_scenes.GetSize(); ++i)
            m_scenes[i]->CleanUp();
        m_scenes.CleanUp();

        Graphics::CleanUpManagers();
        LuaScriptSystem::CleanUpScripts();
    }

    // requires g_engine
    static unsigned int CreateCollisionShape(std::string filename, float scale) { return (unsigned int)g_engine->AddItem<BTCollisionShape>(filename, glm::vec3(scale)); }
    static unsigned int CreateBoxShape(const glm::vec3 &extents) { return (unsigned int)g_engine->AddItem<BTCollisionShape>(new btBoxShape(BulletVector(extents))); }
    static unsigned int CreateSphereShape(float radius) { return (unsigned int)g_engine->AddItem<BTCollisionShape>(new btSphereShape(radius)); }

private:
    Graphics m_graphics;

    Engine m_engine;
    //MyScene m_scene;
    Manager<LuaScene> m_scenes;
    Window* m_window;

    template<class T>
    bool AddComponent(Entity*, luabridge::LuaRef&, COMPONENT_TYPE);
    template<class Arg1, class Arg2, class... Args>
    bool AddComponent(Entity*, luabridge::LuaRef&, COMPONENT_TYPE);

    void GetResources(lua_State*);
};