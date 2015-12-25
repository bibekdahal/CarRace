#pragma once
#include "StandardComponents.h"
#include "../graphics/Graphics.h"

class PointLightComponent : public ComponentBase<POINT_LIGHT_COMPONENT>
{
public:
    PointLightComponent(float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const glm::vec3& color = glm::vec3(1.0f), const glm::vec3 &ambient = glm::vec3(0.4f), bool active = true)
        : m_constantAttenuation(constantAttenuation), m_linearAttenuation(linearAttenuation), m_quadraticAttenuation(quadraticAttenuation),
          m_color(color), m_ambient(ambient), m_active(active) {}
    PointLightComponent(luabridge::LuaRef& component)
    {
        if (!component["Active"].isNil()) m_active = component["Active"].cast<bool>();
        else m_active = true;
        if (!component["ConstantAttenuation"].isNil()) m_constantAttenuation = component["ConstantAttenuation"].cast<float>();
        else m_constantAttenuation = 0.0f;
        if (!component["LinearAttenuation"].isNil()) m_linearAttenuation = component["LinearAttenuation"].cast<float>();
        else m_linearAttenuation = 0.0f;
        if (!component["QuadraticAttenuation"].isNil()) m_quadraticAttenuation = component["QuadraticAttenuation"].cast<float>();
        else m_quadraticAttenuation = 0.2f;
        if (!component["Color"].isNil()) m_color = component["Color"].cast<glm::vec3>();
        else m_color = glm::vec3(1.0f);
        if (!component["Ambient"].isNil()) m_ambient = component["Ambient"].cast<glm::vec3>();
        else m_ambient = glm::vec3(0.4f);
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("pointLight", &Entity::GetComponent<PointLightComponent>)
            .endClass()
            .beginClass<PointLightComponent>("PointLightComponent")
            .addProperty("active", &PointLightComponent::IsActive, &PointLightComponent::SetActive)
            .addProperty("color", &PointLightComponent::GetColor, &PointLightComponent::SetColor)
            .addProperty("ambient", &PointLightComponent::GetAmbient, &PointLightComponent::SetAmbient)
            .addProperty("constantAttenuation", &PointLightComponent::GetConstantAttenuation, &PointLightComponent::SetConstantAttenuation)
            .addProperty("linearAttenuation", &PointLightComponent::GetLinearAttenuation, &PointLightComponent::SetLinearAttenuation)
            .addProperty("quadraticAttenuation", &PointLightComponent::GetQuadraticAttenuation, &PointLightComponent::SetQuadraticAttenuation)
            .endClass();
    }

    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }
    void SetColor(const glm::vec3& color) { m_color = color; }
    glm::vec3 GetColor() const { return m_color; }
    void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    glm::vec3 GetAmbient() const { return m_ambient; }
    void SetConstantAttenuation(float attenuation) { m_constantAttenuation = attenuation; }
    float GetConstantAttenuation() const { return m_constantAttenuation; }
    void SetLinearAttenuation(float attenuation) { m_linearAttenuation = attenuation; }
    float GetLinearAttenuation() const { return m_linearAttenuation; }
    void SetQuadraticAttenuation(float attenuation) { m_quadraticAttenuation = attenuation; }
    float GetQuadraticAttenuation() const { return m_quadraticAttenuation; }

private:
    bool m_active;
    glm::vec3 m_color;
    glm::vec3 m_ambient;

    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;
};

class PointLightSystem : public SystemBase<TRANSFORM_COMPONENT, POINT_LIGHT_COMPONENT>
{
public:
    void Render()
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
        {
            PointLightComponent* light = m_entities[i]->GetComponent<PointLightComponent>();
            TransformComponent* trans = m_entities[i]->GetComponent<TransformComponent>();
            if (light->IsActive())
                Graphics::GetLights().push_back(LightProperties(
                true, 1, light->GetAmbient(), light->GetColor(), glm::vec3(trans->GetWorldTransform()[3]), light->GetConstantAttenuation(), light->GetLinearAttenuation(), light->GetQuadraticAttenuation()
                    ));
        }
    }

private:

};