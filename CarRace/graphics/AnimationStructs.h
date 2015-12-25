#pragma once
#include "../utils/stdinc.h"

struct Key
{
    double time;
};
struct Vec3Key : public Key
{
    glm::vec3 vec3;
};
struct QuatKey : public Key
{
    glm::quat quat;
};
struct KeyFrames
{
    unsigned int nodeId;
    std::vector<Vec3Key> positions;
    std::vector<QuatKey> rotations;
};

struct Animation
{
    double duration, speed;
    std::vector<KeyFrames> keyframesList;
};


struct AnimationNode
{
    unsigned int id;
    std::vector<unsigned int> meshIds;
    AnimationNode* parent;
    std::vector<AnimationNode> children;
};

class TransformComponent;
struct AnimationNodeInfo
{
    TransformComponent* transform;
};

typedef std::vector<AnimationNodeInfo> AnimationNodeInfoMap;