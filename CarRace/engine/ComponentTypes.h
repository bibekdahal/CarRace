#pragma once

typedef uint32_t ComponentTypeMask; //Change this to uint64_t if necessary

// To create a new component, add MY_COMPONENT_TYPE here
// and return TYPE(MY_COMPONENT_TYPE) in component's Type() virtual function
enum COMPONENT_TYPE
{
    TRANSFORM_COMPONENT = 0,
    MODEL_COMPONENT,
    MESH_COMPONENT,
    CAMERA_COMPONENT,

    DIRECTIONAL_LIGHT_COMPONENT,
    POINT_LIGHT_COMPONENT,
    SPOT_LIGHT_COMPONENT,

    BTCOLLIDER_COMPONENT,
    BTRIGIDBODY_COMPONENT,
    BTVEHICLE_COMPONENT,    

    // FINAL
    LUA_SCRIPT_COMPONENT
};

#define TYPEMASK(x) (1 << x)
#define TYPEMASK_INV(x) ((unsigned int)(log(x)/log(2)))

