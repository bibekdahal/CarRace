#pragma once
#include "StandardComponents.h"

class MeshComponent : public ComponentBase<MESH_COMPONENT>
{
public:
    MeshComponent(unsigned int mesh, const glm::vec3& scale=glm::vec3(1.0f)) : m_mesh(mesh), m_scale(scale) {}
    MeshComponent(luabridge::LuaRef component)
    {
        if (component["Mesh"].isNumber()) m_mesh = component["Mesh"].cast<unsigned int>();
        else throw Exception("Mesh component must be created with proper 'Mesh' property", -1);
        if (!component["Scale"].isNil()) m_scale = component["Scale"].cast<glm::vec3>();
        else m_scale = glm::vec3(1.0f);
        if (!component["Visible"].isNil()) m_visible = component["Visible"].cast<bool>();
        else m_visible = true;
        if (component["Material"].isNumber()) m_material = component["Material"].cast<unsigned int>();
        else m_material = 0;
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("mesh", &Entity::GetComponent<MeshComponent>)
            .endClass()
            .beginClass<MeshComponent>("MeshComponent")
            .addProperty("mesh", &GetMesh, &SetMesh)
            .addProperty("scale", &GetScale, &SetScale)
            .addProperty("visible", &IsVisible, &SetVisibility)
            .addProperty("material", &GetMaterial, &SetMaterial)
            .endClass()
            ;
    }

    unsigned int GetMesh() const { return m_mesh; }
    void SetMesh(unsigned int mesh) { m_mesh = mesh; }
    const glm::vec3& GetScale() const { return m_scale; }
    void SetScale(const glm::vec3& scale) { m_scale = scale; }
    bool IsVisible() const { return m_visible; }
    void SetVisibility(bool visibility) { m_visible = visibility; }
    unsigned int GetMaterial() const { return m_material; }
    void SetMaterial(unsigned int mesh) { m_material = mesh; }
private:
    unsigned int m_mesh;
    bool m_visible;
    glm::vec3 m_scale;
    unsigned int m_material;
};

class MeshRenderSystem : public SystemBase<TRANSFORM_COMPONENT, MESH_COMPONENT>
{
public:
    MeshRenderSystem() : m_dirty(false) {}
    void InitEntity(Entity* entity) { m_dirty = true; }
    void DeleteEntity(Entity* entity) { m_dirty = true; }
    void Render()
    {
        if (m_dirty) Refresh();

        glm::mat4 v = Graphics::GetMatrix(0);
        glm::mat4 p = Graphics::GetMatrix(1);
        RenderLayer currentLayer = Graphics::GetCurrentLayer();

        unsigned int start = m_layerMarks[currentLayer].start;
        unsigned int end = m_layerMarks[currentLayer].end;
        Material *lastMat = NULL;
        for (unsigned int i = start; i < end; ++i)
        {
            Entity* entity = m_entities[i];
            MeshComponent * mcomp = entity->GetComponent<MeshComponent>();
            if (!mcomp->IsVisible()) continue;
            TransformComponent * tcomp = entity->GetComponent<TransformComponent>();

            Mesh * mesh = Graphics::GetMeshManager()[mcomp->GetMesh()];
            Material* mat = Graphics::GetMaterialManager()[mcomp->GetMaterial()];
            if (mat != lastMat)
            {
                mat->Switch();
                lastMat = mat;
            }
            mat->Use();
            mesh->Draw(p, v, tcomp->GetWorldTransform() * glm::scale(glm::mat4(), mcomp->GetScale()));
        }
    }

private:
    struct StartEnd
    {
        StartEnd() :start(0), end(0){}
        unsigned int start, end;
    };
    bool m_dirty;
    std::unordered_map<RenderLayer, StartEnd> m_layerMarks;

    static bool CompareLayer(Entity* entity1, Entity* entity2)
    {
        Material * mat1 = Graphics::GetMaterial(entity1->GetComponent<MeshComponent>()->GetMaterial());
        Material * mat2 = Graphics::GetMaterial(entity2->GetComponent<MeshComponent>()->GetMaterial());
        return (entity1->GetLayer() < entity2->GetLayer()) && (mat1->Type() < mat2->Type());
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