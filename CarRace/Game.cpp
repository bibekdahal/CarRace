
#include "Game.h"
#include "lua/LuaComponentRegistration.h"
using namespace luabridge;

inline std::ostream& operator << (std::ostream& os, glm::vec3& v)
{
    os << v.x << " " << v.y << " " << v.z;
    return os;
}
inline std::ostream& operator << (std::ostream& os, glm::mat4& m)
{
    os << "A MATRIX";
    return os;
}

Engine* g_engine;
void Game::GetResources(lua_State* L)
{
    
}



template<class T>
bool GetComponentRef(luabridge::LuaRef& component, unsigned int value, COMPONENT_TYPE ctype)
{
    if (T::Type() != ctype) return false;
    component = (T*)value;
    return true;
}

template<class Arg1, class Arg2, class... Args>
bool GetComponentRef(luabridge::LuaRef& component, unsigned int value, COMPONENT_TYPE ctype)
{
    if (GetComponentRef<Arg1>(component, value, ctype)) return true;
    return GetComponentRef<Arg2, Args...>(component, value, ctype);
}



void Game::Initialize()
{
    m_graphics.Initialize();
    g_engine = &m_engine;

    /////////////////////////////////////////////////////////////////
    // Assets Loading
    /////////////////////////////////////////////////////////////////
    GLSLProgram * program3D = new GLSLProgram;
    std::vector<GLSLProgram::ShaderInfo> shaders = {
        { GL_VERTEX_SHADER, "shaders\\vs_normal3d.glsl" },
        { GL_FRAGMENT_SHADER, "shaders\\fs_normal3d.glsl" },
    };
    std::vector<GLSLProgram::ProgramInfo> programs = {
        { { 0, 1 }, program3D },
    };
    GLSLProgram::CompileShaders(shaders, programs);
    Graphics::GetProgramManager().Add(program3D);
    Graphics::AddMatrix();
    Graphics::AddMatrix();

    DiffuseMaterial* standardMaterial = new DiffuseMaterial;
    Graphics::GetMaterialManager().Add(standardMaterial);
    /*Graphics::GetLights().push_back(LightProperties(true, 0, glm::vec3(0.2f), glm::vec3(0.02f), glm::vec3(0.0f, 0.0f, 1.0f)));
    Graphics::GetLights().push_back(LightProperties(true, 1, glm::vec3(0.2f), glm::vec3(0.6f, 0.0f, 0.0f), 
        glm::vec3(1.5f, -2.0f, -1.0f), 0.00f, 0.001f, 0.02f));
    Graphics::GetLights().push_back(LightProperties(true, 2, glm::vec3(0.2f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-19.0f, -1.5f, -30.0f),
        glm::normalize(glm::vec3(-1.0f, 0.0f, -1.5f)), glm::cos(glm::degrees(60.0f)), 2.0f, 0.01f, 0.01f, 0.02f));*/
    /////////////////////////////////////////////////////////////////



    /////////////////////////////////////////////////////////////////
    
    LuaScript m_script("scripts/game_script.lua");

    lua_State* L = m_script.GetState();
    LuaRef scenes = m_script.GetEnvironment()["Scenes"];
    if (scenes.isNil()) throw GameException("No scene is present", GameException::NO_SCENE);
    int i = 1;
    while (!scenes[i].isNil())
    {
        LuaScene* scene;
        LuaRef luascene = scenes[i++];
        luascene["id"] = m_scenes.Add(scene = new LuaScene(m_window, &m_engine, &m_graphics));
        scene->Initialize();

        LuaRef entities = luascene["Entities"];
        if (entities.isTable())
        {
            // first set id's

            int j = 1;
            while (!entities[j].isNil())
            {
                LuaRef luaentity = entities[j++];
                Entity* entity = new Entity;
                luaentity["id"] = (unsigned int)entity;
                luaentity["type"] = -1;
            }
            j = 1;
            while (!entities[j].isNil())
            {
                LuaRef luaentity = entities[j++];
                Entity* entity = (Entity*)luaentity["id"].cast<unsigned int>();
                if (luaentity.isTable())
                {
                    LuaRef scripts = luaentity["Scripts"];
                    if (scripts.isTable())
                    for (int m = 1; m <= scripts.length(); ++m)
                    {
                        if (!scripts[m].isNumber()) continue;
                        LuaScriptComponent* sc;
                        if (!entity->HasComponent(LUA_SCRIPT_COMPONENT)) sc = entity->AddComponent<LuaScriptComponent>();
                        else sc = entity->GetComponent<LuaScriptComponent>();
                        sc->AddScripts(scripts[m]);
                    }
                    LuaRef properties = luaentity["Properties"];
                    if (properties.isTable())
                    {
                        LuaScriptComponent* sc;
                        if (!entity->HasComponent(LUA_SCRIPT_COMPONENT)) sc = entity->AddComponent<LuaScriptComponent>();
                        else sc = entity->GetComponent<LuaScriptComponent>();

                        for (luabridge::Iterator it(properties); !it.isNil(); ++it)
                        {
                            LuaRef val = it.value(); 
                            std::string key = it.key().cast<std::string>();;
                            if (val.isTable())
                            {
                                if (val["type"] == -1)
                                    sc->GetProperties()[key] = (Entity*)val["id"].cast<unsigned int>();
                                else
                                {
                                    LuaRef l(LuaScript::GetState());
                                    GetComponentRef<COMPONENT_CLASS_LIST>(l, val["id"].cast<unsigned int>(), (COMPONENT_TYPE)val["type"].cast<int>());
                                    sc->GetProperties()[key] = l;
                                }
                                    //if (val["type"] == "Component")
                                    //sc->GetProperties()[key] = (Component*)val["id"].cast<unsigned int>();
                            }
                            else sc->GetProperties()[key] = val;
                        }
                        //for (int i = 1, j = sc->GetProperties().length() + 1; i <= properties.length(); ++i, ++j)
                            //sc->GetProperties()[j] = properties[i];;
                    }
                    for (auto it = LuaScript::GetComponentsMap().begin(); it != LuaScript::GetComponentsMap().end(); ++it)
                    {
                        LuaRef comp = luaentity[it->first];
                        if (comp.isTable())
                            AddComponent<COMPONENT_CLASS_LIST>(entity, comp, (COMPONENT_TYPE)it->second);
                    }
                }
                scene->AddEntity(entity);
            }
        }
    }

    LuaRef sscene = m_script.GetEnvironment()["StartScene"];
    if (!sscene.isTable()) throw Exception("StartScene not specified", -1);
    LuaRef ssid = sscene["id"];
    if (!ssid.isNumber()) throw Exception("StartScene not specified", -1);
    Scene* startScene = m_scenes[ssid];
    m_engine.Init(startScene);

    /////////////////////////////////////////////////////////////////
}



template<class T>
bool Game::AddComponent(Entity* entity, luabridge::LuaRef& component, COMPONENT_TYPE ctype)
{
    if (T::Type() != ctype) return false;
    component["id"] = (unsigned int)entity->AddComponent<T>(component); 
    component["type"] = (int)ctype;
    return true;
}

template<class Arg1, class Arg2, class... Args>
bool Game::AddComponent(Entity* entity, luabridge::LuaRef& component, COMPONENT_TYPE ctype)
{
    if (AddComponent<Arg1>(entity, component, ctype)) return true;
    return AddComponent<Arg2, Args...>(entity, component, ctype);
}

