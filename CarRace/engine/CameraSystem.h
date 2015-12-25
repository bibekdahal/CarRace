#pragma once

#include "StandardComponents.h"
#include "../graphics/Graphics.h"


class CameraSystem : public SystemBase<TRANSFORM_COMPONENT, CAMERA_COMPONENT>
{
public:
    CameraSystem(Graphics* graphics) : m_graphics(graphics) {}
    void Init() 
    {
        m_layers.clear();
        m_nextCamera = 0;
        m_layerIt = m_layers.begin();
        m_passOwnerId = m_graphics->AddPassOwner();
    }

    void InitEntity(Entity* entity)
    {
        CameraComponent* camera = entity->GetComponent<CameraComponent>();
        unsigned int layermask = camera->GetLayersMask();
        for (unsigned int i = 0; i < sizeof(unsigned int)* 8; ++i)
        {
            if ((layermask & 1) == 1)
            {
                m_graphics->SetTotalPasses(m_passOwnerId, m_graphics->GetTotalPasses(m_passOwnerId) + 1);
                m_layers.insert((RenderLayer)i);
            }
            layermask = layermask >> 1;
        }
        RefreshCamera(entity, m_graphics->GetWindowWidth(), m_graphics->GetWindowHeight());

#ifdef _DEBUG
        m_layerIt = m_layers.begin();
#endif
    }

    void DeleteEntity(Entity* entity)
    {
        CameraComponent* camera = entity->GetComponent<CameraComponent>();
        unsigned int layermask = camera->GetLayersMask();
        for (unsigned int i = 0; i < sizeof(unsigned int)* 8; ++i)
        {
            if ((layermask & 1) == 1)
            {
                m_graphics->SetTotalPasses(m_passOwnerId, m_graphics->GetTotalPasses(m_passOwnerId) - 1);;
                m_layers.erase((RenderLayer)i);
            }
            layermask = layermask >> 1;
        }
    }
    
    void Resize(float width, float height)
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
            RefreshCamera(m_entities[i], width, height);
    }

    void Render()
    {
        if (m_graphics->GetCurrentPassOwner() != m_passOwnerId) return;
        if (m_layers.size() == 0) return;
        CameraComponent* camera;
        RenderLayer layer;
        Entity* entity;
        while (true)
        {
            entity = m_entities[m_nextCamera];
            camera = entity->GetComponent<CameraComponent>();
            layer = *m_layerIt;
            m_nextCamera++;
            if (m_nextCamera == m_entities.size())
            {
                m_layerIt++;
                if (m_layerIt == m_layers.end()) m_layerIt = m_layers.begin();
                m_nextCamera = 0;
            }

            if (camera->IsActive() && camera->HasLayer(layer)) break;
        }

        // active camera is "camera", layer is "layer"
        if (camera->GetDirty()) 
            RefreshCamera(entity, m_graphics->GetWindowWidth(), m_graphics->GetWindowHeight());
        Graphics::SetCurrentLayer(layer);
        Graphics::GetMatrix(0) = glm::affineInverse(entity->GetComponent<TransformComponent>()->GetWorldTransform());
        Graphics::GetMatrix(1) = camera->GetMatrix();
        Rect v = camera->GetViewport();
        m_graphics->ChangeViewport((int)(v.left*m_graphics->GetWindowWidth()), (int)(v.bottom*m_graphics->GetWindowHeight()),
            (int)(v.width*m_graphics->GetWindowWidth()), (int)(v.height*m_graphics->GetWindowHeight()));
    }

private:
    Graphics* m_graphics;
    unsigned int m_nextCamera;
    std::set<RenderLayer> m_layers;
    std::set<RenderLayer>::iterator m_layerIt;
    unsigned int m_passOwnerId;

    void RefreshCamera(Entity* entity, float width, float height)
    {
        CameraComponent* camera = entity->GetComponent<CameraComponent>();
        Rect v = camera->GetViewport();
        float aspect = (v.width*width) / (v.height*height);
        glm::mat4 proj = glm::perspective(glm::radians(camera->GetFOV()), aspect, camera->GetNearPlane(), camera->GetFarPlane());
        camera->SetMatrix(proj);
        camera->SetDirty(false);
    }
};
