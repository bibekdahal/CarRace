#pragma once

#include "../graphics/Window.h"
#include "System.h"


class Graphics;
class Engine;
class Scene
{
public:
    Scene(Window* window, Engine* engine, Graphics* graphics) : m_window(window), m_engine(engine), m_graphics(graphics) {}
    virtual ~Scene() {}

    void CleanUp() 
    {
        Finish();
        for (unsigned i = 0; i < m_systems.GetSize(); ++i)
            m_systems[i]->CleanUp();
        m_systems.CleanUp();

        m_entities.CleanUp();
    }
    void Initialize()
    {
        Init();
    }
    void InitializeSystems()
    {
        for (unsigned i = 0; i < m_systems.GetSize(); ++i)
            m_systems[i]->Initialize();
    }

    Manager<void>& GetStore() { return m_store; }
    Manager<System>& GetSystems() { return m_systems; }
    Manager<Entity>& GetEntities() { return m_entities; }

    unsigned int AddEntity(Entity* entity, bool addToSystems = true);


    virtual void Init() {};
    virtual void Finish() {}
protected:
    Manager<void> m_store;
    Manager<System> m_systems;
    Manager<Entity> m_entities;
    Window* m_window;
    Engine* m_engine;
    Graphics* m_graphics;
};