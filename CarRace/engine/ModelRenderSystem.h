#pragma once

#include "StandardComponents.h"
#include "../graphics/Model.h"


class ModelRenderSystem : public SystemBase<TRANSFORM_COMPONENT, MODEL_COMPONENT>
{
public:
    ModelRenderSystem() : m_dirty(false) {}
    void InitEntity(Entity* entity) { m_dirty = true; }
    void DeleteEntity(Entity* entity) { m_dirty = true; }
    void Render()
    {
        if (m_dirty) Refresh();

        glm::mat4 v = Graphics::GetMatrix(0);
        glm::mat4 p = Graphics::GetMatrix(1);
        RenderLayer currentLayer = Graphics::GetCurrentLayer();
        
        std::unordered_map<Material*, bool> matUsed;
        
        unsigned int start = m_layerMarks[currentLayer].start;
        unsigned int end = m_layerMarks[currentLayer].end;
        for (unsigned int i = start; i < end; ++i)
        {
            Entity* entity = m_entities[i];
            ModelComponent * mcomp = entity->GetComponent<ModelComponent>();
            if (!mcomp->IsVisible()) continue;
            TransformComponent * tcomp = entity->GetComponent<TransformComponent>();

            Model * model = Graphics::GetModelManager()[mcomp->GetModel()];
            for (unsigned j = 0; j < model->GetMeshesSize(); ++j)
            {
                Mesh * mesh = Graphics::GetMeshManager()[model->GetMesh(j)];
                Material* mat = Graphics::GetMaterialManager()[model->GetMaterial(j)];
                if (!matUsed[mat])
                {
                    mat->Switch();
                    matUsed[mat] = true;
                }
                mat->Use();
                mesh->Draw(p, v, tcomp->GetWorldTransform() * glm::scale(glm::mat4(), mcomp->GetScale()) * model->GetOffset(j));
            }
        }
    }

private:
    struct StartEnd 
    { 
        StartEnd() :start(0),end(0){} 
        unsigned int start, end; 
    };
    bool m_dirty;
    std::unordered_map<RenderLayer, StartEnd> m_layerMarks;

    static bool CompareLayer(Entity* entity1, Entity* entity2)
    {
        return entity1->GetLayer() < entity2->GetLayer();
    }
    void Refresh()
    {
        m_dirty = false;
        std::sort(m_entities.begin(), m_entities.end(), CompareLayer);
        m_layerMarks.clear();
        RenderLayer layer = m_entities[0]->GetLayer();
        m_layerMarks[layer].start = 0;
        unsigned int i;
        for (i = 1; i < m_entities.size(); ++i)
        {
            RenderLayer newLayer = m_entities[i]->GetLayer();
            if (layer != newLayer)
            {
                m_layerMarks[layer].end = i;
                layer = newLayer;
                m_layerMarks[layer].start = i;
            }
        }
        m_layerMarks[layer].end = i;
    }
};