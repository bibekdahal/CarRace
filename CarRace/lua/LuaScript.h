#pragma once
#include "lua.h"
#include "../utils/stdinc.h"
#include "../engine/StandardComponents.h"
#include "../BTPhysics/BTVehicle.h"

class LuaScriptException : public Exception {
public:
    enum LUASCRIPTEXCEPTIONTYPE{ FAIL_RUN_SCRIPT };
    LuaScriptException(std::string errorString, int errorType) : Exception(errorString, errorType) {}
};

enum PROPERTY_TYPES {
 INT, FLOAT, BOOL, DOUBLE, CHAR, STRING, VEC3, MAT4, QUAT, ENTITY,
    PROPERTY_NUMS
};
extern const char* PROPERTY_NAMES[];
extern std::string g_property;

class LuaScriptComponent;
class LuaCall;
class LuaScript
{
public:
    LuaScript(std::string filename);
    static void Init();
    static void CleanUp();
    static void AddBasics(Window * window);
    static void AddMaths();
    static void AddEntity();
    static void AddComponents();
    static void AddMaterials();

    static void AddEverything(Window* window)
    {
        AddBasics(window);
        AddMaterials();
        AddMaths();
        AddEntity();
        AddComponents();
    }
    //static void AddComponentDefinitions();
    //void GetComponents(std::vector<COMPONENT_TYPE> &components);
    //void GetProperties(std::unordered_map<std::string, int> &properties);
    //void AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int> &properties);

    static lua_State* GetState() { return L; }
    static std::map<COMPONENT_TYPE, const void*> GetKeys() { return m_keys; }

    LuaCall CreateCall(const std::string &function_name);
    luabridge::LuaRef& GetEnvironment() { return m_environment; }

    static void RunScript(std::string script)
    {
        if (luaL_dostring(L, script.c_str()))
        {
            std::string error = "Failed to run script" + std::string(lua_tostring(L, -1));
            lua_pop(L, 1);
            throw LuaScriptException(error, LuaScriptException::FAIL_RUN_SCRIPT);
        }
    }
    static void RunScriptFromFile(std::string filename)
    {
        if (luaL_dofile(L, filename.c_str()))
        {
            std::string error = "Failed to run script" + std::string(lua_tostring(L, -1));
            lua_pop(L, 1);
            throw LuaScriptException(error, LuaScriptException::FAIL_RUN_SCRIPT);
        }
    }

    static std::map<std::string, int>& GetComponentsMap() { return m_components; }
private:
    luabridge::LuaRef m_environment;


    static lua_State *L;
    static std::map<COMPONENT_TYPE, const void*> m_keys;
    static std::map<std::string, int> m_components;

    /*template<class T>
    bool AddProperty(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it, int C, const T& sample);
    template<class T>
    bool AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it);
    template<class Arg1, class Arg2, class... Args>
    bool AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it);*/

    template<class T>
    static void AddComponent()
    {
        T::RegisterToLua(L);
        m_keys[T::Type()] = luabridge::ClassInfo<T>::getClassKey();
    }
    template<class T1, class T2, class... Ts>
    static void AddComponent()
    {
        AddComponent<T1>();
        AddComponent<T2, Ts...>();
    }

    template<class T>
    static void AddMaterial()
    {
        T::RegisterToLua(L);
    }
    template<class T1, class T2, class... Ts>
    static void AddMaterial()
    {
        AddMaterial<T1>();
        AddMaterial<T2, Ts...>();
    }
};



#include "LuaCall.h"