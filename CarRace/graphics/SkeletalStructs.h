#pragma once
#include "AnimationStructs.h"

struct Bone
{
    unsigned int nodeId;
    glm::mat4 offset;
};

struct SkinInfo
{
    unsigned int boneIds[4];
    float weights[4];
};