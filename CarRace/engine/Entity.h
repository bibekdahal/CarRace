#pragma once

#include "../utils/stdinc.h"
#include "ComponentTypes.h"

class Entity;

class Component
{
public:
    virtual ComponentTypeMask TypeMask() = 0;
    virtual bool AttachToEntity(Entity* entity) { return true; }
    virtual ~Component() {}
};

template<COMPONENT_TYPE type>
class ComponentBase : public Component
{
public:
    virtual ComponentTypeMask TypeMask() { return TYPEMASK(type); }
    static COMPONENT_TYPE Type() { return type; }
};


#include "../graphics/Layers.h"
class Entity
{
public:
    Entity(RenderLayer layer = DefaultLayer) : m_cbitmap(0), m_layer(layer) {}
    virtual ~Entity()
    {
        for (auto it = m_components.begin(); it != m_components.end(); ++it)
            delete it->second;
    }
    template<class T, COMPONENT_TYPE componentType, class... Args>
    T* AddComponent(Args... args)
    {
        T* component = new T(args...);
        if (!component->AttachToEntity(this))
        {
            delete component;
            return NULL;
        }
        m_components[componentType] = component;
        m_cbitmap |= TYPEMASK(componentType);
        return component;
    }
    template<class T, class... Args>
    T* AddComponent(Args... args)
    {
        T* component = new T(args...);
        if (!component->AttachToEntity(this))
        {
            delete component;
            return NULL;
        }
        m_components[T::Type()] = component;
        m_cbitmap |= TYPEMASK(T::Type());
        return component;
    }

    void AddType(COMPONENT_TYPE type) { m_cbitmap |= TYPEMASK(type); }

    template<class T>
    T* GetComponent() const { return static_cast<T*>(m_components.at(T::Type())); }
    Component* GetComponent(COMPONENT_TYPE type) const { return m_components.at(type); }

    ComponentTypeMask GetComponentsTypes() { return m_cbitmap; }
    bool HasComponent(COMPONENT_TYPE type) { return (m_cbitmap & TYPEMASK(type)) == TYPEMASK(type); }

    RenderLayer GetLayer() const { return m_layer; }
protected:
    std::unordered_map<COMPONENT_TYPE, Component*> m_components;
    ComponentTypeMask m_cbitmap;
    RenderLayer m_layer;
};