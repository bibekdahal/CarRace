#pragma once

class LuaCall
{
public:
    LuaCall(luabridge::LuaRef &env, const std::string &function_name)
        : m_function(env[function_name]) {}

    /*void operator()(std::vector<int> &types, std::vector<void*> &params)
    {
        m_function.push(m_L);
        for (unsigned int i = 0; i < params.size(); ++i)
            PushParam(types[i], params[i]);
        m_paramsSize = (int)params.size();
    }*/
    template <class... Args>
    luabridge::LuaRef operator()(Args... args)
    {
        return m_function(args...);
    }
    void BeginCall()
    {
        m_function.push(LuaScript::GetState());
        m_paramsSize = 0;
    }
    luabridge::LuaRef EndCall()
    {
        luabridge::LuaException::pcall(LuaScript::GetState(), m_paramsSize, 1);
        return luabridge::LuaRef::fromStack(LuaScript::GetState());
    }

    template<class T>
    void PushParam(T param)
    {
        luabridge::Stack<T>::push(LuaScript::GetState(), param);
        m_paramsSize++;
    }

    void PushParam(void* param, const void* key)
    {
        luabridge::UserdataPtr::push_ex(LuaScript::GetState(), param, key);
        m_paramsSize++;
    }

    operator bool const()
    {
        return !m_function.isNil();
    }
private:
    luabridge::LuaRef m_function;
    int m_paramsSize;

   /* void PushParam(int type, void* param)
    {
#define PUSHAS(x) luabridge::Stack<x>::push(m_L, reinterpret_cast<x>(param))
        switch (type)
        {
        case TRANSFORM_COMPONENT:
            PUSHAS(TransformComponent*); break;
        case MODEL_COMPONENT:
            PUSHAS(ModelComponent*); break;
        case CAMERA_COMPONENT:
            PUSHAS(CameraComponent*); break;
        case BTCOLLIDER_COMPONENT:
            PUSHAS(BTColliderComponent*); break;
        case BTRIGIDBODY_COMPONENT:
            PUSHAS(BTRigidBodyComponent*); break;
        case BTVEHICLE_COMPONENT:
            PUSHAS(BTVehicleComponent*); break;
        }
#undef PUSH
#undef PUSHAS_B
    }*/
};