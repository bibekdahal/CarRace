#pragma once

#include "../engine/entity.h"
#include "LuaScript.h"

enum CALLS
{
    LOAD_SCRIPT = 0, FINISH_SCRIPT, INIT, FINISH, RESIZE, UPDATE, 
    CALLS_NUM
};

extern const char* CALL_NAMES[];
class LuaScriptComponent;
struct LuaScriptInfo
{
    LuaScriptInfo(Window* window, std::string filename);
    ~LuaScriptInfo() { if (calls[FINISH_SCRIPT]) calls[FINISH_SCRIPT](); }

    LuaScript script;
    //std::vector<COMPONENT_TYPE> types;
    std::vector<LuaCall> calls;
    //std::unordered_map<std::string, int> properties;
    //void AddProperties(luabridge::LuaRef &luaProperties);
};

class LuaScriptComponent : public ComponentBase<LUA_SCRIPT_COMPONENT>
{
public:
    LuaScriptComponent() : m_properties(luabridge::newTable(LuaScript::GetState())) {}

    unsigned int AddScripts(unsigned int script);
    void AddScripts(unsigned int script1, unsigned int scripts...) { AddScripts(script1); AddScripts(scripts); }
    unsigned int GetScript(unsigned int index) { return m_scripts[index]; }
    unsigned int GetScriptsSize() { return m_scripts.size(); }

    luabridge::LuaRef& GetProperties() { return m_properties; }
    //luabridge::LuaRef& GetLuaEntity(unsigned int index) { return m_luaEntities[index]; }
private:
    //std::unordered_map<std::string, PropertyBase*> m_properties;
    //std::vector<luabridge::LuaRef> m_luaEntities;
    luabridge::LuaRef m_properties;
    std::vector<unsigned int> m_scripts;

    friend LuaScript;
};