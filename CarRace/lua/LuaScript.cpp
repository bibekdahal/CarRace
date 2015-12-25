#define LUA_SCRIPT_CPP

//#include "LuaScript.h"
//#include "LuaScriptSystem.h"
#include "../graphics/Model.h"
#include "../Game.h"
#include "LuaComponentRegistration.h"
#include "LuaMaterialRegistrations.h"
using namespace luabridge;

lua_State *LuaScript::L;
std::map<COMPONENT_TYPE, const void*> LuaScript::m_keys;

const char* PROPERTY_NAMES[] =
{ "int", "float", "bool", "double", "char", "string", "vec3", "mat4", "quat", "Entity" };

LuaScript::LuaScript(std::string filename)
: m_environment(L)
{
    auto foo = getGlobal(L, "LoadEnvironment");
    m_environment = foo(filename);
}

void LuaScript::Init()
{ 
    L = luaL_newstate(); 
    luaL_openlibs(L);

    std::string script =
        "function LoadEnvironment(file)  \n\
            local newenv = {}   \n\
            setmetatable(newenv, { __index = _G })  \n\
            assert(loadfile(file, \"bt\", newenv))()  \n\
            return newenv   \n\
        end";
    RunScript(script);
}

void LuaScript::CleanUp() 
{ 
    lua_close(L); 
}

LuaCall LuaScript::CreateCall(const std::string &function_name) { return LuaCall(m_environment, function_name); }

static Window* g_window;
void LuaScript::AddBasics(Window* window)
{
    unsigned int(*AddScript)(std::string) = ([](std::string fname) { /*static Window* window = g_window;*/ return LuaScriptSystem::AddScript(g_window, fname); });
    bool(*CheckKey)(int) = ([](int key) { /*static Window* window = g_window;*/ return g_window->CheckKey(key); });
    g_window = window;

    getGlobalNamespace(L)
        .beginClass<Window>("Window")
        .addStaticFunction("CheckKey", CheckKey)
        .endClass()

        .beginClass<Rect>("Rect")
        .addConstructor<void(*)(float, float, float, float)>()
        .addData("left", &Rect::left)
        .addData("bottom", &Rect::bottom)
        .addData("width", &Rect::width)
        .addData("height", &Rect::height)
        .endClass()

        .addFunction("AddScript", AddScript)
        .addFunction("AddModel", &Model::AddModel)
        .addFunction("AddModelEx", &Model::AddModelEx)
        .addFunction("AddBoxMesh", &Model::AddBoxMesh)
        .addFunction("AddSphereMesh", &Model::AddSphereMesh)
        .addFunction("AddShape", &Game::CreateCollisionShape)
        .addFunction("AddBoxShape", &Game::CreateBoxShape)
        .addFunction("AddSphereShape", &Game::CreateSphereShape)

        .addFunction("DuplicateModel", &Model::Duplicate)

        .beginClass<Mesh>("Mesh")
        .endClass()
        .beginClass<Model::MeshInfo>("MeshInfo")
        .addData("mesh", &Model::MeshInfo::mesh)
        .addData("material", &Model::MeshInfo::material)
        .addData("offset", &Model::MeshInfo::offset)
        .endClass()
        .beginClass<Model>("Model")
        .addFunction("GetMeshInfo", &Model::GetMeshInfo)
        .addFunction("GetMaterial", &Model::GetMaterial)
        .addFunction("GetMesh", &Model::GetMesh)
        .addFunction("GetOffset", &Model::GetOffset)
        .addFunction("SetMaterial", &Model::SetMaterial)
        .addFunction("SetMesh", &Model::SetMesh)
        .addFunction("SetOffset", &Model::SetOffset)
        .addProperty("numMeshes", &Model::GetMeshesSize)
        .endClass()

        .addFunction("GetModel", &Model::GetModel)
        .addFunction("GetMesh", &Model::GetMeshPtr)
        ;
}


#include "lua_glm_wrapper.h"
void LuaScript::AddMaths()
{
    typedef glm::mat4(*MAT4_FUNC)(const glm::mat4&);
    typedef glm::mat3(*MAT3_FUNC)(const glm::mat3&);

    getGlobalNamespace(L)
        .addFunction("Degrees", (float(*)(const float&))&glm::degrees)
        .addFunction("Radians", (float(*)(const float&))&glm::radians)

        .beginClass<glm::vec3>("vec3")
        .addConstructor<void(*)(float, float, float)>()
        .addData("x", &glm::vec3::x)
        .addData("y", &glm::vec3::y)
        .addData("z", &glm::vec3::z)
        .addProperty("degrees", &VEC3_DEG, (void(*)(glm::vec3*, glm::vec3))0)
        .addProperty("radians", &VEC3_RAD, (void(*)(glm::vec3*, glm::vec3))0)
        .addStaticFunction("Add", &VEC3_ADD)
        .addStaticFunction("Subtract", &VEC3_SUBTRACT)
        .addStaticFunction("Multiple", &VEC3_MULTIPLY)
        .addStaticFunction("Scale", &VEC3_SCALE)
        .endClass()

        .beginClass<glm::vec4>("vec4")
        .addConstructor<void(*)(float, float, float, float)>()
        .addData("x", &glm::vec4::x)
        .addData("y", &glm::vec4::y)
        .addData("z", &glm::vec4::z)
        .addData("w", &glm::vec4::w)
        .addProperty("degrees", &VEC4_DEG, (void(*)(glm::vec4*, glm::vec4))0)
        .addProperty("radians", &VEC4_RAD, (void(*)(glm::vec4*, glm::vec4))0)
        .addStaticFunction("Add", &VEC4_ADD)
        .addStaticFunction("Subtract", &VEC4_SUBTRACT)
        .addStaticFunction("Multiple", &VEC4_MULTIPLY)
        .addStaticFunction("Scale", &VEC4_SCALE)
        .endClass()

        .beginClass<glm::quat>("quat")
        .addConstructor<void(*)(float, float, float, float)>()
        .addData("x", &glm::quat::x)
        .addData("y", &glm::quat::y)
        .addData("z", &glm::quat::z)
        .addData("w", &glm::quat::z)
        .addProperty("mat4", &GET_MAT_4, (void(*)(glm::quat*, glm::mat4))0)
        .addProperty("vec4", &GET_VEC_4, (void(*)(glm::quat*, glm::vec4))0)
        .addProperty("Euler", &EULER_QUAT, (void(*)(glm::quat*, glm::vec3))0)
        .addStaticFunction("Add", &QUAT_ADD)
        .addStaticFunction("Multiply", &QUAT_MULTIPLY)
        .addStaticFunction("Scale", &QUAT_SCALE)
        .addStaticFunction("Rotate", &ROTATE_QUAT)
        .endClass()
        
        .beginClass<glm::mat3>("mat3")
        .addStaticFunction("Inverse", (MAT3_FUNC)&glm::inverse)
        .addStaticFunction("Transpose", (MAT3_FUNC)&glm::transpose)
        .addStaticFunction("AffineInverse", (MAT3_FUNC)&glm::affineInverse)
        .addProperty("vec0", &GET_VEC_3<0>, (void(*)(glm::mat3*, glm::vec3))0)
        .addProperty("vec1", &GET_VEC_3<1>, (void(*)(glm::mat3*, glm::vec3))0)
        .addProperty("vec2", &GET_VEC_3<2>, (void(*)(glm::mat3*, glm::vec3))0)
        .addProperty("mat4", &GET_MAT_4, (void(*)(glm::mat3*, glm::mat4))0)
        .addStaticFunction("Add", &MAT3_ADD)
        .addStaticFunction("Subtract", &MAT3_SUBTRACT)
        .addStaticFunction("Multiple", &MAT3_MULTIPLY)
        .addStaticFunction("Scale", &MAT3_SCALE)
        .addStaticFunction("Transform", &MAT3_TRANSFORM)
        .endClass()

        .beginClass<glm::mat4>("mat4")
        .addConstructor<void(*)()>()
        .addStaticFunction("Inverse", (MAT4_FUNC)&glm::inverse)
        .addStaticFunction("Transpose", (MAT4_FUNC)&glm::transpose)
        .addStaticFunction("AffineInverse", (MAT4_FUNC)&glm::affineInverse)
        .addProperty("vec0", &GET_VEC_4<0>, (void(*)(glm::mat4*, glm::vec4))0)
        .addProperty("vec1", &GET_VEC_4<1>, (void(*)(glm::mat4*, glm::vec4))0)
        .addProperty("vec2", &GET_VEC_4<2>, (void(*)(glm::mat4*, glm::vec4))0)
        .addStaticFunction("Add", &MAT4_ADD)
        .addStaticFunction("Subtract", &MAT4_SUBTRACT)
        .addStaticFunction("Multiple", &MAT4_MULTIPLY)
        .addStaticFunction("Scale", &MAT4_SCALE)
        .addStaticFunction("Transform", &MAT4_TRANSFORM)

        .addStaticFunction("LookAt", &glm::lookAt<float>)
        .addProperty("mat3", &GET_MAT_3, (void(*)(glm::mat4*, glm::mat3))0)
        .addProperty("quat", &GET_QUAT, (void(*)(glm::mat4*, glm::quat))0)
        .addStaticFunction("Rotate", &ROTATE_MAT)
        .addStaticFunction("Scale", &SCALE_MAT)
        .addStaticFunction("Translate", &TRANSLATE_MAT)
        .endClass()
        ;
}

static LuaRef GetProperties(const Entity* self) { return self->GetComponent<LuaScriptComponent>()->GetProperties(); }
void LuaScript::AddEntity()
{
    getGlobalNamespace(L)
        .beginClass<Entity>("Entity")
        .addFunction("HasComponent", &Entity::HasComponent)
        .addProperty("layer", &Entity::GetLayer)
        .addProperty("properties", &GetProperties, (void(*)(Entity*, LuaRef))0)
        .endClass();
}
void LuaScript::AddComponents()
{
    AddComponent<COMPONENT_CLASS_LIST>();
}
void LuaScript::AddMaterials()
{
    getGlobalNamespace(L)
        .beginClass<Material>("Material")
        .addProperty("type", &Material::Type)
        .endClass()
        .addFunction("GetMaterial", &Graphics::GetMaterial);
    AddMaterial<MATERIAL_CLASS_LIST>();
}

//void LuaScript::AddComponentDefinitions()
//{
//    std::string script = "";
//    for (auto it = m_components.begin(); it != m_components.end(); ++it)
//        script += it->first + "=" + std::to_string(it->second) + "\n";
//    RunScript(script);
//}

//void LuaScript::GetComponents(std::vector<COMPONENT_TYPE> &components)
//{
//    luabridge::LuaRef cmpts = luabridge::getGlobal(L, "Components");
//    if (cmpts.isNil()) return;
//    int i = 1;
//    while (!cmpts[i].isNil())
//        components.push_back((COMPONENT_TYPE)(cmpts[i++].cast<int>()));
//}
//
//void LuaScript::GetProperties(std::unordered_map<std::string, int> &properties)
//{
//    luabridge::LuaRef propts = luabridge::getGlobal(L, "Properties");
//    if (propts.isNil()) return;
//    int i = 1;
//    while (!propts[i].isNil())
//    {
//        std::string property = propts[i++].cast<std::string>();
//        unsigned c = property.find(':');
//        std::string name = property.substr(0, c);
//        std::string type = property.substr(c + 1); 
//        unsigned int j;
//        for (j = 0; j < PROPERTY_NUMS; ++j)
//        if (type == PROPERTY_NAMES[j])
//            properties[name] = j;
//        if (j == PROPERTY_NUMS)
//        for (auto it = m_components.begin(); it != m_components.end(); ++it)
//        if (type == it->first)
//            properties[name] = it->second + PROPERTY_NUMS;
//    }
//}
//
//static std::string g_property;
//void LuaScript::AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int> &properties)
//{
//    getGlobalNamespace(L).beginClass<LuaScriptComponent>("EntityProperties").endClass();
//    for (auto it = properties.begin(); it != properties.end(); ++it)
//    {
//        switch (it->second)
//        {
//#define ADD_PROPERTY(y, x, sample) case y: AddProperty<x>(sc, it, y, sample); break
//#define ADD_PROPERTY_EX(y, x) ADD_PROPERTY(y, x, x())
//
//            ADD_PROPERTY(INT, int, 0);
//            ADD_PROPERTY(FLOAT, float, 0.0f);
//            ADD_PROPERTY(BOOL, bool, 0);
//            ADD_PROPERTY(DOUBLE, double, 0.0);
//            ADD_PROPERTY(CHAR, char, 0);
//            ADD_PROPERTY(STRING, std::string, "");
//            ADD_PROPERTY_EX(VEC3, glm::vec3);
//            ADD_PROPERTY_EX(MAT4, glm::mat4);
//            ADD_PROPERTY(ENTITY, Entity*, NULL);
//
//        default:
//            AddProperties<COMPONENT_CLASS_LIST>(sc, it);
//        }
//#undef ADD_PROPERTY
//#undef ADD_PROPERTY_EX
//    }
//}

//template<class T>
//void SetGlobalVariable(lua_State* L, std::string name, T data)
//{
//    Stack<T>::push(L, data);
//    lua_setglobal(L, name.c_str());
//}

//template<class T>
//void SetGlobal(lua_State* L, const std::string &name, T** value)
//{
//    push(L, *value);
//    lua_setglobal(L, name.c_str());
//}
//template<class T>
//void SetGlobal(lua_State* L, const std::string &name, T* value)
//{
//    push(L, value);
//    lua_setglobal(L, name.c_str());
//}

//
//template<class T>
//inline bool LuaScript::AddProperty(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it, int C, const T& sample)
//{
//    if (it->second == C)
//    {
//        //LuaScriptComponent* sc = new LuaScriptComponent;
//        /*g_property = it->first;
//        T(*Get)(LuaScriptComponent const*) = ([](LuaScriptComponent const* t) { static std::string prop = g_property; return t->GetData<T>(prop); });
//        void(*Set)(LuaScriptComponent*, T) = ([](LuaScriptComponent* t, T data) { static std::string prop = g_property; t->SetData<T>(prop, data); });
//        getGlobalNamespace(L).beginClass<LuaScriptComponent>("EntityProperties").addProperty(it->first.c_str(), Get, Set).endClass();
//        sc->AddProperty<T>(it->first); Get(sc); Set(sc, sample);*/
//        //delete sc;
//        
//        getGlobalNamespace(L).beginNamespace("P").addVariable(it->first.c_str(), 
//            &((Property<T>*)(sc->m_properties[it->first]))->m_data).endNamespace();
//        
//        
//        //SetGlobal(L, it->first, &(((Property<T>*)(sc->m_properties[it->first]))->m_data));
//        
//        return true;
//    }
//    return false;
//}
//
//template<class T>
//inline bool LuaScript::AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it)
//{
//    return AddProperty<T*>(sc, it, (int)T::Type() + PROPERTY_NUMS, NULL);
//}
//
//template<class Arg1, class Arg2, class... Args>
//inline bool LuaScript::AddProperties(LuaScriptComponent* sc, std::unordered_map<std::string, int>::iterator &it)
//{
//    if (AddProperties<Arg1>(sc, it)) return true;
//    return AddProperties<Arg2, Args...>(sc, it);
//}

#undef LUA_SCRIPT_CPP