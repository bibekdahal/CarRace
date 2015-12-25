#pragma once

#include "Entity.h"

class System
{
public:
    System() : m_componentTypes(0), m_initialized(false) {}
    virtual ~System() {}

    void Initialize()
    {
        Init();
        for (unsigned i = 0; i < m_entities.size(); ++i)
            InitEntity(m_entities[i]);
        m_initialized = true;
    }
    virtual void Init() {}
    virtual void Update(double deltaTime) {}
    virtual void Resize(float width, float height) {}
    virtual void Render() {}
    virtual void CleanUp() {}

    bool AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);

    virtual void InitEntity(Entity* entity) {}
    virtual void DeleteEntity(Entity* entity) {}
protected:
    std::vector<Entity*> m_entities;
    ComponentTypeMask m_componentTypes;
    bool m_initialized;
};

inline ComponentTypeMask GetTypeMask(COMPONENT_TYPE type) { return TYPEMASK(type); }
inline ComponentTypeMask GetTypeMask(COMPONENT_TYPE type, COMPONENT_TYPE types...) { return TYPEMASK(type) | GetTypeMask(types); }

template<COMPONENT_TYPE... types>
class SystemBase : public System
{
public:
    SystemBase<types...>() { m_componentTypes = GetTypeMask(types...); }
};