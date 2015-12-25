#pragma once

#include "../engine/Scene.h"
#include "../engine/ModelRenderSystem.h"
#include "../engine/MeshRenderSystem.h"
#include "../engine/CameraSystem.h"
#include "../engine/DirectionalLightSystem.h"
#include "../engine/PointLightSystem.h"
#include "../engine/SpotLightSystem.h"
#include "../lua/LuaScriptSystem.h"

class LuaScene : public Scene
{
public:
    LuaScene(Window* window, Engine* engine, Graphics* graphics) : Scene(window, engine, graphics) {}
    void Init()
    {
        m_systems.Add(new CameraSystem(m_graphics));
        m_systems.Add(new DirectionalLightSystem);
        m_systems.Add(new PointLightSystem);
        m_systems.Add(new SpotLightSystem);
        m_systems.Add(new MeshRenderSystem);
        m_systems.Add(new ModelRenderSystem);
        unsigned int p = m_systems.Add(new BTPhysicsSystem);
        m_systems.Add(new BTVehicleSystem((BTPhysicsSystem*)m_systems[p]));
        m_systems.Add(new LuaScriptSystem());
        InitializeSystems();
    }

private:

};