#include "LuaScriptSystem.h"

const char* CALL_NAMES[] =
{ "LoadScript", "FinishScript", "Init", "Finish", "Resize", "Update" };


LuaScriptInfo::LuaScriptInfo(Window* window, std::string filename)
: script(filename)
{
    for (unsigned i = 0; i < CALLS_NUM; ++i)
        calls.push_back(script.CreateCall(CALL_NAMES[i]));
    if (calls[LOAD_SCRIPT]) calls[LOAD_SCRIPT]();
}
//
//void LuaScriptInfo::AddProperties(luabridge::LuaRef &entity)
//{
//    for (auto it = properties.begin(); it != properties.end(); ++it)
//    {
//#define ADD_PROPERTY(y, x) entity //case y: sc->AddProperty<x>(it->first); break
//        switch (it->second)
//        {
//            ADD_PROPERTY(INT, int);
//            ADD_PROPERTY(FLOAT, float);
//            ADD_PROPERTY(BOOL, bool);
//            ADD_PROPERTY(DOUBLE, double);
//            ADD_PROPERTY(CHAR, char);
//            ADD_PROPERTY(STRING, std::string);
//            ADD_PROPERTY(VEC3, glm::vec3);
//            ADD_PROPERTY(MAT4, glm::mat4);
//            ADD_PROPERTY(QUAT, glm::quat);
//            ADD_PROPERTY(ENTITY, Entity*);
//
//            // Consider all other as components
//        default:
//            //sc->AddProperty<Component*>(it->first);
//        }
//#undef ADD_PROPERTY
//#undef ADD_PROPERTY_C
//    }
//}

unsigned int LuaScriptComponent::AddScripts(unsigned int script) 
{ 
    m_scripts.push_back(script);
    //m_properties = new
    //luabridge::LuaRef(LuaScriptSystem::GetScripts()[script]->script.GetState(), m_entity);
    //LuaScriptSystem::GetScripts()[script]->AddProperties(*m_properties); 
    return m_scripts.size() - 1; 
}
