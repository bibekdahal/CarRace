#pragma once

#include "Scene.h"

class Engine
{
public:
    void Init(Scene* newscene)
    {
        m_currentScene = newscene;
    }
    void CleanUp()
    {
        m_manager.CleanUp();
    }
    void Resize(float width, float height)
    {
        for (unsigned j = 0; j < m_currentScene->GetSystems().GetSize(); ++j)
            m_currentScene->GetSystems()[j]->Resize(width, height);
    }

    void Update(double deltaTime)
    {
        for (unsigned j = 0; j < m_currentScene->GetSystems().GetSize(); ++j)
            m_currentScene->GetSystems()[j]->Update(deltaTime);
    }
    void Render()
    {
        for (unsigned j = 0; j < m_currentScene->GetSystems().GetSize(); ++j)
            m_currentScene->GetSystems()[j]->Render();
    }
   
    Scene* GetCurrentScene() { return m_currentScene; }
    void SwitchScene(Scene * newscene)
    {
        m_currentScene = newscene;
    }

    Manager<Manageable>& GetManager() { return m_manager; }
    template<class T, class... Args>
    T* AddItem(Args... args) 
    {
        T* item = new T(args...);
        m_manager.Add(item);
        return item;
    }
    template<class T>
    T* GetItem(unsigned int index) { return static_cast<T*>(m_manager[index]); }

private:
    Scene* m_currentScene;
    Manager<Manageable> m_manager;
};