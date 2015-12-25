#pragma once

#include "../engine/DirectionalLightSystem.h"
#include "../engine/PointLightSystem.h"
#include "../engine/SpotLightSystem.h"
#include "../engine/MeshRenderSystem.h"

#define COMPONENT_CLASS_LIST   \
    TransformComponent, ModelComponent, MeshComponent, CameraComponent \
    , DirectionalLightComponent, PointLightComponent, SpotLightComponent \
    , BTColliderComponent, BTRigidBodyComponent, BTVehicleComponent
/* REGISTRATION OF COMPONENT CLASSES */

// To keep things simple, start in new line with a comma and end with a back slash
// e.g.
//  , MYComponent   \


#ifdef LUA_SCRIPT_CPP
std::map<std::string, int> LuaScript::m_components = {
    { "Transform", TRANSFORM_COMPONENT },
    { "Model", MODEL_COMPONENT },
    { "Mesh", MESH_COMPONENT },
    { "Camera", CAMERA_COMPONENT },

    { "DirectionalLight", DIRECTIONAL_LIGHT_COMPONENT },
    { "PointLight", POINT_LIGHT_COMPONENT },
    { "SpotLight", SPOT_LIGHT_COMPONENT },

    { "BTCollider", BTCOLLIDER_COMPONENT },
    { "BTRigidBody", BTRIGIDBODY_COMPONENT },
    { "BTVehicle", BTVEHICLE_COMPONENT },
    /* REGISTRATION OF COMPONENT FOR SCRIPT-CONSTANTS */
};
#endif
