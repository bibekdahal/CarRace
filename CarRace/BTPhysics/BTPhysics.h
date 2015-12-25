#pragma once

#include "../engine/StandardComponents.h"
#include "btBulletDynamicsCommon.h"
#include "D:/bullet-2.82-r2704/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"

inline btVector3 BulletVector(const glm::vec3 &v) { return btVector3(v.x, v.y, v.z); }
inline glm::vec3 FromBulletVector(const btVector3 &v) { return glm::vec3(v.x(), v.y(), v.z()); }
class BTCollisionShape : public Manageable
{
public:
    BTCollisionShape(btCollisionShape* shape) : m_shape(shape) {}
    BTCollisionShape(std::string filename, const glm::vec3 &scaling=glm::vec3(1.0f))
    {
        btBulletWorldImporter import(0);            //don't store info into the world
        if (import.loadFile(filename.c_str()))
        {
            int numShape = import.getNumCollisionShapes();
            if (numShape)
            {
                m_shape = (btBvhTriangleMeshShape*)import.getCollisionShapeByIndex(0);
                m_shape->setLocalScaling(btVector3(scaling.x, scaling.y, scaling.z));
            }
        }
    }
    ~BTCollisionShape() { delete m_shape; }
    btCollisionShape* GetCollisionShape() { return m_shape; }
private:
    btCollisionShape* m_shape;
};

class BTColliderComponent : public ComponentBase<BTCOLLIDER_COMPONENT>
{
public:
    BTColliderComponent(BTCollisionShape* shape) : m_collisionShape(shape) {}
    BTColliderComponent(luabridge::LuaRef &component) 
    { 
        if (component["Shape"].isNumber()) m_collisionShape = (BTCollisionShape*)component["Shape"].cast<unsigned int>(); 
        else m_collisionShape = NULL;
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("btCollider", &Entity::GetComponent<BTColliderComponent>)
            .endClass()
            .beginClass<BTColliderComponent>("BTCollider")
            .endClass();
    }
    btCollisionShape* GetCollisionShape() { return m_collisionShape->GetCollisionShape(); }

protected:
    BTCollisionShape* m_collisionShape;
};

class BTRigidBodyComponent : public ComponentBase<BTRIGIDBODY_COMPONENT>, btMotionState
{
public:
    BTRigidBodyComponent(float mass) : m_mass(mass), m_rigidBody(NULL), m_transformComponent(NULL) {}
    BTRigidBodyComponent(luabridge::LuaRef &component)
    {
        if (component["Mass"].isNumber()) m_mass = component["Mass"];
        else m_mass = 0.0f;
        m_rigidBody = NULL; m_transformComponent = NULL;
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("btRigidBody", &Entity::GetComponent<BTRigidBodyComponent>)
            .endClass()
            .beginClass<BTRigidBodyComponent>("BTRigidBody")
            .addFunction("ApplyTorque", &ApplyTorque)
            .addFunction("ApplyForce", &ApplyForce)
            .addFunction("ApplyCentralForce", &ApplyCentralForce)
            .addFunction("ApplyImpulse", &ApplyImpulse)
            .addFunction("ApplyCentralImpulse", &ApplyCentralImpulse)
            .addFunction("ApplyTorqueImpulse", &ApplyTorqueImpulse)
            .addFunction("ClearForces", &ClearForces)
            .addProperty("linearVelocity", &GetLinearVeloctiy, &SetLinearVelocity)
            .addProperty("angularVelocity", &GetAngularVeloctiy, &SetAngularVelocity)
            .addFunction("Activate", &Activate)
            .addFunction("DisableDeactivation", &DisableDeactivation)
            .endClass();
    }
    ~BTRigidBodyComponent() { delete m_rigidBody; }
    btRigidBody* GetRigidBody() const { return m_rigidBody; }

    void ApplyTorque(const glm::vec3 &torque) { m_rigidBody->applyTorque(BulletVector(torque)); }
    void ApplyForce(const glm::vec3 &force, const glm::vec3 &rel_pos) { m_rigidBody->applyForce(BulletVector(force), BulletVector(rel_pos)); }
    void ApplyCentralForce(const glm::vec3 &force) { m_rigidBody->applyCentralForce(BulletVector(force)); }
    void ApplyImpulse(const glm::vec3 &impulse, const glm::vec3 &rel_pos) { m_rigidBody->applyImpulse(BulletVector(impulse), BulletVector(rel_pos)); }
    void ApplyCentralImpulse(const glm::vec3 &impulse) { m_rigidBody->applyCentralImpulse(BulletVector(impulse)); }
    void ApplyTorqueImpulse(const glm::vec3 &torque) { m_rigidBody->applyTorqueImpulse(BulletVector(torque)); }
    void ClearForces() { m_rigidBody->clearForces(); }
    
    void SetLinearVelocity(const glm::vec3 &velocity) { m_rigidBody->setLinearVelocity(BulletVector(velocity)); }
    void SetAngularVelocity(const glm::vec3 &velocity) { m_rigidBody->setAngularVelocity(BulletVector(velocity)); }
    glm::vec3 GetLinearVeloctiy() const { return FromBulletVector(m_rigidBody->getLinearVelocity()); }
    glm::vec3 GetAngularVeloctiy() const { return FromBulletVector(m_rigidBody->getAngularVelocity()); }

    void Activate() { m_rigidBody->activate(); }
    void DisableDeactivation() { m_rigidBody->setActivationState(DISABLE_DEACTIVATION); }

    void CreateRigidBody(Entity* entity)
    {
        if (m_transformComponent || m_rigidBody) return;
        m_transformComponent = entity->GetComponent<TransformComponent>();
        btCollisionShape* shape = entity->GetComponent<BTColliderComponent>()->GetCollisionShape();
        btVector3 inertia(0, 0, 0);
        if (m_mass != 0) shape->calculateLocalInertia(m_mass, inertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(m_mass, this, shape, inertia);
        m_rigidBody = new btRigidBody(rigidBodyCI);
    }
protected:
    btRigidBody* m_rigidBody;
    TransformComponent* m_transformComponent;
    float m_mass;

    virtual void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans.setFromOpenGLMatrix(glm::value_ptr(m_transformComponent->GetWorldTransform())); 
    }
    virtual void setWorldTransform(const btTransform &worldTrans)
    {
        glm::mat4 trans;
        worldTrans.getOpenGLMatrix(glm::value_ptr(trans));
        m_transformComponent->SetWorldTransform(trans);
    }
};

//typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionPair;
//typedef std::set<CollisionPair> CollisionPairs;
class BTPhysicsSystem : public SystemBase<BTCOLLIDER_COMPONENT>
{
public:
    void Init()
    {
        m_broadphase = new btDbvtBroadphase();
        m_collisionConfiguration = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
        m_solver = new btSequentialImpulseConstraintSolver;
        m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
        SetGravity(-10.0f);
    }

    btDiscreteDynamicsWorld* GetWorld() const { return m_dynamicsWorld; }
    void SetGravity(float gravity) { m_dynamicsWorld->setGravity(btVector3(0, gravity, 0)); }

    void InitEntity(Entity* entity)
    {
        if (entity->HasComponent(BTRIGIDBODY_COMPONENT))
        {
            BTRigidBodyComponent* rgb = entity->GetComponent<BTRigidBodyComponent>();
            rgb->CreateRigidBody(entity);
            m_dynamicsWorld->addRigidBody(rgb->GetRigidBody());
        }
    }
    void DeleteEntity(Entity* entity)
    {
        if (entity->HasComponent(BTRIGIDBODY_COMPONENT))
        {
            BTRigidBodyComponent* rgb = entity->GetComponent<BTRigidBodyComponent>();
            m_dynamicsWorld->removeRigidBody(rgb->GetRigidBody());
        }
    }

    void Update(double deltaTime)
    {
        m_dynamicsWorld->stepSimulation(static_cast<float>(deltaTime));
        //CheckForCollisionEvents();
    }

    void CleanUp()
    {
        for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
            m_dynamicsWorld->removeCollisionObject(obj);
        }

        delete m_dynamicsWorld;
        delete m_solver;
        delete m_collisionConfiguration;
        delete m_dispatcher;
        delete m_broadphase;
    }

protected:
    btBroadphaseInterface* m_broadphase;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;

    /*CollisionPairs m_pairsLastUpdate;
    void CollisionEvent(btRigidBody * pBody0, btRigidBody * pBody1)
    {
        RigidUnit * one = (RigidUnit*)pBody0->getUserPointer();
        RigidUnit * two = (RigidUnit*)pBody1->getUserPointer();
        one->Collision(two); two->Collision(one);
    }
    void SeparationEvent(btRigidBody * pBody0, btRigidBody * pBody1)
    {
        RigidUnit * one = (RigidUnit*)pBody0->getUserPointer();
        RigidUnit * two = (RigidUnit*)pBody1->getUserPointer();
        one->Separation(two); two->Separation(one);
    }*/
};
