#include "Engine.h"

bool System::AddEntity(Entity* entity)
{
    if ((entity->GetComponentsTypes() & m_componentTypes) == m_componentTypes)
    {
        m_entities.push_back(entity);
        if (m_initialized) InitEntity(entity);
        return true;
    }
    return false;
}
void System::RemoveEntity(Entity* entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end())
    {
        m_entities.erase(it);
        DeleteEntity(entity);
    }
}