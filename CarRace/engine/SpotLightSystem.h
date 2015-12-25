#pragma once
#include "StandardComponents.h"
#include "../graphics/Graphics.h"

class SpotLightComponent : public ComponentBase<SPOT_LIGHT_COMPONENT>
{
public:
    SpotLightComponent(float cutOffAngle, float exponent, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const glm::vec3& color = glm::vec3(1.0f), const glm::vec3 &ambient = glm::vec3(0.4f), bool active = true)
        : m_cutOff(glm::cos(cutOffAngle)), m_exponent(exponent),
        m_constantAttenuation(constantAttenuation), m_linearAttenuation(linearAttenuation), m_quadraticAttenuation(quadraticAttenuation),
        m_color(color), m_ambient(ambient), m_active(active) {}
    SpotLightComponent(luabridge::LuaRef& component)
    {
        if (!component["Active"].isNil()) m_active = component["Active"].cast<bool>();
        else m_active = true;

        if (!component["CutOffAngle"].isNil()) m_cutOff = glm::cos(glm::radians(component["CutOffAngle"].cast<float>()));
        else if (!component["CosCutOff"].isNil()) m_cutOff = component["CosCutOff"].cast<float>();
        else m_cutOff = glm::cos(glm::radians(60.0f));
        if (!component["Exponent"].isNil()) m_exponent = component["Exponent"].cast<float>();
        else m_exponent = 2.0f;

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
            .addProperty("spotLight", &Entity::GetComponent<SpotLightComponent>)
            .endClass()
            .beginClass<SpotLightComponent>("SpotLightComponent")
            .addProperty("active", &SpotLightComponent::IsActive, &SpotLightComponent::SetActive)
            .addProperty("color", &SpotLightComponent::GetColor, &SpotLightComponent::SetColor)
            .addProperty("ambient", &SpotLightComponent::GetAmbient, &SpotLightComponent::SetAmbient)
            .addProperty("cutOffAngle", &SpotLightComponent::GetCutOffAngle, &SpotLightComponent::SetCutOffAngle)
            .addProperty("cosCutOff", &SpotLightComponent::GetCosCutOff, &SpotLightComponent::SetCosCutOff)
            .addProperty("exponent", &SpotLightComponent::GetExponent, &SpotLightComponent::SetExponent)
            .addProperty("constantAttenuation", &SpotLightComponent::GetConstantAttenuation, &SpotLightComponent::SetConstantAttenuation)
            .addProperty("linearAttenuation", &SpotLightComponent::GetLinearAttenuation, &SpotLightComponent::SetLinearAttenuation)
            .addProperty("quadraticAttenuation", &SpotLightComponent::GetQuadraticAttenuation, &SpotLightComponent::SetQuadraticAttenuation)
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

    void SetCutOffAngle(float angle) { m_cutOff = glm::cos(glm::radians(angle)); }
    float GetCutOffAngle() const { return glm::degrees(glm::acos(m_cutOff)); }
    void SetExponent(float exp) { m_exponent = exp; }
    float GetExponent() const { return m_exponent; }

    void SetCosCutOff(float cutOff) { m_cutOff = cutOff; }
    float GetCosCutOff() const { return m_cutOff; }
private:
    bool m_active;
    glm::vec3 m_color;
    glm::vec3 m_ambient;

    float m_cutOff;
    float m_exponent;

    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;
};

class SpotLightSystem : public SystemBase<TRANSFORM_COMPONENT, SPOT_LIGHT_COMPONENT>
{
public:
    void Render()
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
        {
            SpotLightComponent* light = m_entities[i]->GetComponent<SpotLightComponent>();
            TransformComponent* trans = m_entities[i]->GetComponent<TransformComponent>();
            if (light->IsActive())
            {   
                glm::mat4 world = trans->GetWorldTransform();
                Graphics::GetLights().push_back(LightProperties(
                    true, 2, light->GetAmbient(), light->GetColor(), glm::vec3(world[3]),
                    glm::normalize(glm::mat3(world) * glm::vec3(1.0f, 0.0f, 0.0f)), light->GetCosCutOff(), light->GetExponent(),
                    light->GetConstantAttenuation(), light->GetLinearAttenuation(), light->GetQuadraticAttenuation()
                    ));
            }
        }
    }

private:

};