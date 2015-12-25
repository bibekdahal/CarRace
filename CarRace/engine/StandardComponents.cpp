#include "StandardComponents.h"
#include "Engine.h"

void TransformComponent::SetParent(TransformComponent* parentComponent)
{
    if (!parentComponent) return;
    if (parentComponent->TypeMask() != TYPEMASK(TRANSFORM_COMPONENT))
        parentComponent = NULL;
    TransformComponent* oldParent = m_parent;
    
    m_parent = parentComponent;
    if (m_parent)
        m_parent->m_children.push_back(this);

    if (oldParent)
    {
        auto it = std::find(oldParent->m_children.begin(), oldParent->m_children.end(), this);
        *it = *oldParent->m_children.end();
        oldParent->m_children.pop_back();
    }
}


#include "../graphics/Model.h"
ModelComponent::~ModelComponent()
{
    if (m_nodes)
    {
        //for (auto it = m_nodes->begin(); it != m_nodes->end(); ++it)
            //delete it->second.transform;
        delete m_nodes;
    }
}
static void AssignNode(AnimationNode* node, AnimationNodeInfoMap& nodes, TransformComponent* transform)
{
    AnimationNodeInfo info;
    info.transform = transform;
    nodes[node->id] = info;
    unsigned int num = glm::min(node->children.size(), transform->GetChildrenSize());
    for (unsigned int i = 0; i < num; ++i)
        AssignNode(&node->children[i], nodes, transform->GetChild(i));
}

void ModelComponent::AssignMap(TransformComponent* rootTrans)
{
    if (!rootTrans)
    {
        m_nodes = NULL;
        return;
    }
    Model* model = Model::GetModel(m_model);
    AnimationNode* root = model->GetRootNode();
    if (root)
    {        
        m_nodes = new AnimationNodeInfoMap;
        m_nodes->resize(model->GetNumNodes());
        AssignNode(root, *m_nodes, rootTrans);
    }
}

