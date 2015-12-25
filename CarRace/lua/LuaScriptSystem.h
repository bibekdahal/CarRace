#pragma once

#include "../engine/System.h"
#include "LuaScriptComponent.h"


class LuaScriptSystem : public SystemBase<LUA_SCRIPT_COMPONENT>
{
public:
    void InitEntity(Entity* entity)
    {
        for (unsigned int i = 0; i < entity->GetComponent<LuaScriptComponent>()->GetScriptsSize(); ++i)
        {
            LuaScriptInfo* lua = m_scripts[entity->GetComponent<LuaScriptComponent>()->GetScript(i)];
            lua->script.GetEnvironment()["self"] = entity;
            if (lua->calls[INIT])
            {
                lua->calls[INIT].BeginCall();
                //PushEntity(lua, INIT, entity);
                lua->calls[INIT].EndCall();
            }
        }
    }
    void DeleteEntity(Entity* entity)
    {
        for (unsigned int i = 0; i < entity->GetComponent<LuaScriptComponent>()->GetScriptsSize(); ++i)
        {
            LuaScriptInfo* lua = m_scripts[entity->GetComponent<LuaScriptComponent>()->GetScript(i)];
            lua->script.GetEnvironment()["self"] = entity;
            if (!lua->calls[FINISH]) continue;
            lua->calls[FINISH].BeginCall();
            //PushEntity(lua, FINISH, entity);
            lua->calls[FINISH].EndCall();
        }
    }

    void Resize(float width, float height)
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
        {
            Entity* entity = m_entities[i];
            for (unsigned int j = 0; j < entity->GetComponent<LuaScriptComponent>()->GetScriptsSize(); ++j)
            {
                LuaScriptInfo* lua = m_scripts[entity->GetComponent<LuaScriptComponent>()->GetScript(j)];
                lua->script.GetEnvironment()["self"] = entity;
                if (!lua->calls[RESIZE]) continue;
                lua->calls[RESIZE].BeginCall();
                lua->calls[RESIZE].PushParam(width);
                lua->calls[RESIZE].PushParam(height);
                //PushEntity(lua, RESIZE, entity);;
                lua->calls[RESIZE].EndCall();
            }
        }
    }
    void Update(double deltaTime)
    {
        for (unsigned int i = 0; i < m_entities.size(); ++i)
        {
            Entity* entity = m_entities[i];
            for (unsigned int j = 0; j < entity->GetComponent<LuaScriptComponent>()->GetScriptsSize(); ++j)
            {
                LuaScriptInfo* lua = m_scripts[entity->GetComponent<LuaScriptComponent>()->GetScript(j)];
                lua->script.GetEnvironment()["self"] = entity;
                if (!lua->calls[UPDATE]) continue;
                lua->calls[UPDATE].BeginCall();
                lua->calls[UPDATE].PushParam(deltaTime);
                //PushEntity(lua, UPDATE, entity);
                lua->calls[UPDATE].EndCall();
            }
        }
    }

    void PushEntity(LuaScriptInfo* script, int callid, Entity* entity)
    {
        //script->script.AddProperties(entity->GetComponent<LuaScriptComponent>(), script->properties);
        //for (unsigned i = 0; i < script->types.size(); ++i)
            //script->calls[callid].PushParam(entity->GetComponent(script->types[i]), script->script.GetKeys()[script->types[i]]);
        //script->calls[callid].PushParam(entity->GetComponent<LuaScriptComponent>());
    }

    static Manager<LuaScriptInfo>& GetScripts() { return m_scripts; }
    static unsigned int AddScript(Window* window, std::string filename) { return m_scripts.Add(new LuaScriptInfo(window, filename)); }
    static void CleanUpScripts() { m_scripts.CleanUp(); }
private:
    static Manager<LuaScriptInfo> m_scripts;
};