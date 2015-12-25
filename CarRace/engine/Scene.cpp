#pragma once
#include "Scene.h"

unsigned int Scene::AddEntity(Entity* entity, bool addtoSystems)
{
    unsigned int id = m_entities.Add(entity);
    if (addtoSystems)
    for (unsigned i = 0; i < m_systems.GetSize(); ++i)
        m_systems[i]->AddEntity(entity);
    return id;
}