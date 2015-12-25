#pragma once
#define NUM_LIGHTS 10

struct LightProperties
{
    LightProperties(bool isEnabled, int type, const glm::vec3 &ambient, const glm::vec3 &color, const glm::vec3 &position, const glm::vec3 &coneDirection,
    float spotCosCutoff, float spotExponent, float constantAttenuation, float linearAttenuation, float quadraticAttenuation)
    : isEnabled(isEnabled), type(type), ambient(ambient), color(color), position(position), coneDirection(coneDirection),
    spotCosCutoff(spotCosCutoff), spotExponent(spotExponent), constantAttenuation(constantAttenuation), linearAttenuation(linearAttenuation),
    quadraticAttenuation(quadraticAttenuation)
    {}
    LightProperties(bool isEnabled = true) : isEnabled(isEnabled) {}

    //Directional
    LightProperties(bool isEnabled, int type, const glm::vec3 &ambient, const glm::vec3 &color, const glm::vec3 &direction)
        : isEnabled(isEnabled), type(type), ambient(ambient), color(color), position(direction) {}
    //Point
    LightProperties(bool isEnabled, int type, const glm::vec3 &ambient, const glm::vec3 &color, const glm::vec3 &position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation)
        : isEnabled(isEnabled), type(type), ambient(ambient), color(color), position(position),
        constantAttenuation(constantAttenuation), linearAttenuation(linearAttenuation), quadraticAttenuation(quadraticAttenuation)
    {}
   
    bool isEnabled;
    int type;
    glm::vec3 ambient;
    glm::vec3 color;
    glm::vec3 position;
    //glm::vec3 halfVector;
    glm::vec3 coneDirection;
    float spotCosCutoff;
    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};