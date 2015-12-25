#pragma once
#include "StandardComponents.h"
#include "../graphics/Graphics.h"

class DirectionalLightComponent : public ComponentBase<DIRECTIONAL_LIGHT_COMPONENT>
{
public:
    DirectionalLightComponent(const glm::vec3& direction, const glm::vec3& color=glm::vec3(1.0f), const glm::vec3 &ambient=glm::vec3(0.4f), bool active=true)
        : m_direction(glm::normalize(direction)), m_color(color), m_ambient(ambient), m_active(active) {}
    DirectionalLightComponent(luabridge::LuaRef& component)
    {
        if (!component["Active"].isNil()) m_active = component["Active"].cast<bool>();
        else m_active = true;
        if (!component["Direction"].isNil()) m_direction = glm::normalize(component["Direction"].cast<glm::vec3>());
        else m_direction = glm::vec3(-1.0f, 0.0f, 0.0f);
        if (!component["Color"].isNil()) m_color = component["Color"].cast<glm::vec3>();
        else m_color = glm::vec3(1.0f);
        if (!component["Ambient"].isNil()) m_ambient = component["Ambient"].cast<glm::vec3>();
        else m_ambient = glm::vec3(0.4f);
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("directionalLight", &Entity::GetComponent<DirectionalLightComponent>)
            .endClass()
            .beginClass<DirectionalLightComponent>("DirectionalLightComponent")
            .addProperty("active", &DirectionalLightComponent::IsActive, &DirectionalLightComponent::SetActive)
            .addProperty("direction", &DirectionalLightComponent::GetDirection, &DirectionalLightComponent::SetDirection)
            .addProperty("color", &DirectionalLightComponent::GetColor, &DirectionalLightComponent::SetColor)
            .addProperty("ambient", &DirectionalLightComponent::GetAmbient, &DirectionalLightComponent::SetAmbient)
            .endClass();
    }

    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }
    void SetDirection(const glm::vec3& direction) { m_direction = glm::normalize(direction); }
    glm::vec3 GetDirection() const { return m_direction; }
    void SetColor(const glm::vec3& color) { m_color = color; }
    glm::vec3 GetColor() const { return m_color; }
    void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    glm::vec3 GetAmbient() const { return m_ambient; }

private:
    bool m_active;
    glm::vec3 m_direction;
    glm::vec3 m_color;
    glm::vec3 m_ambient;
};

class DirectionalLightSystem : public SystemBase<DIRECTIONAL_LIGHT_COMPONENT>
{
public:
    void Render()
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
        {
            DirectionalLightComponent* light = m_entities[i]->GetComponent<DirectionalLightComponent>();
            if (light->IsActive())
                Graphics::GetLights().push_back(LightProperties(true, 0, light->GetAmbient(), light->GetColor(), -light->GetDirection()));
        }
    }

private:

};