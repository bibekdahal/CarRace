#pragma once

#include "Engine.h"
#include "../lua/lua.h"

class TransformComponent : public ComponentBase<TRANSFORM_COMPONENT>
{
public:
    TransformComponent() : m_dirty(false), m_transform(1.0f), m_position(0.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f), m_parent(NULL) {}
    TransformComponent(luabridge::LuaRef &component)
    {
        using namespace luabridge;
        m_dirty = false;
        m_transform = glm::mat4(1.0f);

        LuaRef pos = component["Position"];
        if (!pos.isNil()) SetPosition(pos.cast<glm::vec3>());
        else m_position = glm::vec3(0.0f);

        LuaRef rot = component["Rotation"];
        if (!rot.isNil()) SetRotation(rot.cast<glm::quat>());
        else m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        LuaRef parent = component["Parent"];
        if (parent.isTable()) m_parent = (TransformComponent*)parent["id"].cast<unsigned int>();
        else m_parent = NULL;
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("transform", &Entity::GetComponent<TransformComponent>)
            .endClass()
            .beginClass<TransformComponent>("TransformComponent")
            .addProperty("parent", &GetParent, &SetParent)
            .addProperty("numChildren", &GetChildrenSize)
            .addFunction("GetPosition", &GetPosition)
            .addFunction("SetPosition", &SetPosition)
            .addFunction("GetRotation", &GetRotation)
            .addFunction("SetRotation", &SetRotation)
            .addFunction("GetWorldTransform", &GetWorldTransform)
            .addFunction("SetWorldTransform", &SetWorldTransform)
            .addFunction("GetChild", &GetChild)
            .endClass();
    }
    
    glm::mat4 GetLocalTransform() { return glm::translate(glm::mat4(), m_position) * glm::mat4_cast(m_rotation); }
    const glm::mat4& GetWorldTransform()
    {
        if (m_dirty)
        {
            if (m_parent) m_transform = m_parent->GetWorldTransform() * GetLocalTransform();
            else  m_transform = GetLocalTransform();
            m_dirty = false;
        }
        return m_transform;
    }
    void SetWorldTransform(glm::mat4& transform)
    {
        m_transform = transform;
        if (m_parent)
            transform = glm::/*inverse*/affineInverse(m_parent->GetWorldTransform()) * transform;
        m_position = glm::vec3(transform[3]);
        transform[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_rotation = glm::quat_cast(transform);
        SetDirty(true);
    }

    void SetPosition(const glm::vec3& position) { m_position = position; SetDirty(); }
    void SetRotation(const glm::quat& rotation) { m_rotation = rotation; SetDirty(); }
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::quat& GetRotation() const { return m_rotation; }
   
    void SetParent(TransformComponent* parentComponent);
    TransformComponent* GetParent() const { return m_parent; }
    unsigned int GetChildrenSize() const { return m_children.size(); }
    TransformComponent* GetChild(unsigned int index) const { return m_children[index]; }

    void SetDirty(bool forchildrenonly = false)
    {
        if (!forchildrenonly) m_dirty = true;
        for (unsigned i = 0; i < m_children.size(); ++i)
            m_children[i]->SetDirty();
    }

protected:
    glm::mat4 m_transform;
    glm::vec3 m_position;
    glm::quat m_rotation;
    bool m_dirty;
    TransformComponent* m_parent;
    std::vector<TransformComponent*> m_children;
};

struct AnimationNodeInfo;
typedef std::vector<AnimationNodeInfo> AnimationNodeInfoMap;
class ModelComponent : public ComponentBase<MODEL_COMPONENT>
{
public:
    ModelComponent(unsigned int model, TransformComponent* root=NULL) : m_scale(1.0f), m_model(model), m_visible(true) 
    { AssignMap(root); }
    ModelComponent(luabridge::LuaRef &component)
    {
        if (component["Model"].isNumber()) m_model = component["Model"];
        else throw Exception("Model component must be created with proper 'Model' property", -1);
        if (component["Scale"].isNumber()) m_scale = glm::vec3(component["Scale"].cast<float>());
        else m_scale = glm::vec3(1.0f);
        if (!component["Visible"].isNil()) m_visible = component["Visible"].cast<bool>();
        else m_visible = true;

        luabridge::LuaRef root = component["RootNode"];
        TransformComponent* rootNode = NULL;
        if (root.isTable()) rootNode = (TransformComponent*)root["RootNode"].cast<unsigned int>();
        AssignMap(rootNode);
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("model", &Entity::GetComponent<ModelComponent>)
            .endClass()
            .beginClass<ModelComponent>("ModelComponent")
            .addProperty("model", &GetModel, &SetModel)
            .addProperty("visible", &IsVisible, &SetVisibility)
            .addProperty("scale", &GetScale, &SetScale)
            .endClass();
    }
    ~ModelComponent();

    void SetModel(unsigned int model) { m_model = model; }
    unsigned int GetModel() const { return m_model; }

    void SetScale(const glm::vec3& scale) { m_scale = scale; }
    const glm::vec3& GetScale() const { return m_scale; }

    bool IsVisible() const { return m_visible; }
    void SetVisibility(bool visibility) { m_visible = visibility; }

    AnimationNodeInfoMap* GetNodeTransformMap() { return m_nodes; }
protected:
    unsigned int m_model;
    glm::vec3 m_scale;
    bool m_visible;
    AnimationNodeInfoMap * m_nodes;

    void AssignMap(TransformComponent* root);
};


inline unsigned int CreateMaskedLayers(RenderLayer layer)
{
    return 1 << layer;
}
inline unsigned int CreateMaskedLayers(RenderLayer layer1, RenderLayer layers...)
{
    return CreateMaskedLayers(layer1) | CreateMaskedLayers(layers);
}

struct Rect
{
    Rect(float l, float b, float w, float h) : left(l), bottom(b), width(w), height(h) {}
    float left, bottom, width, height;
};
class CameraComponent : public ComponentBase<CAMERA_COMPONENT>
{
public:
    CameraComponent(bool isActive = true) : 
        m_layers(1 << DefaultLayer), m_active(isActive),
        m_fov(60), m_near(0.1f), m_far(10000.0f), m_dirty(true), m_viewport(0.0f, 0.0f, 1.0f, 1.0f)
    {}
    CameraComponent(luabridge::LuaRef& component) : CameraComponent()
    {
        if (!component["Active"].isNil()) m_active = component["Active"].cast<bool>();
        if (component["FOV"].isNumber()) m_fov = component["FOV"];
        if (component["Near"].isNumber()) m_near = component["Near"];
        if (component["Far"].isNumber()) m_far = component["Far"];
        if (!component["Viewport"].isNil()) m_viewport = component["Viewport"].cast<Rect>();
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("camera", &Entity::GetComponent<CameraComponent>)
            .endClass()
            .beginClass<CameraComponent>("CameraComponent")
            //.addFunction("SetLayers", &CameraComponent::SetLayers)
            .addFunction("GetLayersMask", &GetLayersMask)
            .addFunction("AddLayer", &AddLayer)
            .addFunction("RemoveLayer", &RemoveLayer)
            .addFunction("HasLayer", &HasLayer)
            .addProperty("fov", &GetFOV, &SetFOV)
            .addProperty("near_plane", &GetNearPlane, &SetNearPlane)
            .addProperty("far_plane", &GetFarPlane, &SetFarPlane)
            .addProperty("dirty", &GetDirty, &SetDirty)
            .addFunction("GetViewport", &GetViewport)
            .addFunction("SetViewport", &SetViewport)
            .endClass();
    }

    void SetLayers(RenderLayer layers...) { m_layers = CreateMaskedLayers(layers); }
    unsigned int GetLayersMask() { return m_layers; }
    void AddLayer(RenderLayer layer) { m_layers |= (1 << layer); }
    void RemoveLayer(RenderLayer layer) { m_layers &= ~(1 << layer); }
    bool HasLayer(RenderLayer layer) { return (m_layers & (1 << layer)) == (1 << layer); }
    
    bool IsActive() { return m_active; }
    void SetActive(bool isActive) { m_active = isActive; }

    const glm::mat4 &GetMatrix() const { return m_vp; }
    void SetMatrix(const glm::mat4& matrix) { m_vp = matrix; }

    void SetFOV(float fov) { m_fov = fov; m_dirty = true; }
    float GetFOV() const { return m_fov; }
    void SetNearPlane(float distance) { m_near = distance; m_dirty = true; }
    float GetNearPlane() const { return m_near; }
    void SetFarPlane(float distance) { m_far = distance; m_dirty = true; }
    float GetFarPlane() const { return m_far; }

    const Rect& GetViewport() const { return m_viewport; }
    void SetViewport(const Rect& viewport) { m_dirty = true;  m_viewport = viewport; }

    bool GetDirty() const { return m_dirty; }
    void SetDirty(bool dirty) { m_dirty = dirty; }
protected:
    unsigned int m_layers;
    bool m_active;
    bool m_dirty;

    float m_fov, m_near, m_far;
    glm::mat4 m_vp;

    Rect m_viewport;
};