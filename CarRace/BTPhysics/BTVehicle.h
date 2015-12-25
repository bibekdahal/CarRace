#pragma once

#include "BTPhysics.h"

#define BTG(a) (btVector3(a.x,a.y,a.z))
class BTVehicleComponent : public ComponentBase<BTVEHICLE_COMPONENT>
{
public:
    BTVehicleComponent() : m_vehicle(NULL), m_tmpWheels(new std::vector<TmpWheelInfo>) {}
    BTVehicleComponent(luabridge::LuaRef &component)
    {
        m_vehicle = NULL;
        m_tmpWheels = new std::vector<TmpWheelInfo>;
        luabridge::LuaRef wheels = component["Wheels"];
        if (wheels.isTable())
        {
            int i = 1;
            while (wheels[i].isTable())
            {
                TmpWheelInfo info;
                luabridge::LuaRef luainfo = wheels[i++];
                luabridge::LuaRef wheel = luainfo["Construct"];
                if (wheel.isTable())
                {
                    if (!wheel[1].isNil()) info.m_connectionPoint = wheel[1].cast<glm::vec3>();
                    if (!wheel[2].isNil()) info.m_wheelDirection = wheel[2].cast<glm::vec3>();
                    if (!wheel[3].isNil()) info.m_wheelAxle = wheel[3].cast<glm::vec3>();
                    if (!wheel[4].isNil()) info.m_suspensionRestLength = wheel[4].cast<float>();
                    if (!wheel[5].isNil()) info.m_wheelRadius = wheel[5].cast<float>();
                    if (!wheel[6].isNil()) info.m_isFrontWheel = wheel[6].cast<bool>();
                }
                WheelInfo info2;
                luabridge::LuaRef entity = luainfo["Entity"];
                if (entity.isTable() && entity["id"].isNumber()) info2.entity = (Entity*)entity["id"].cast<unsigned int>();
                if (!luainfo["Reflect"].isNil()) info2.reflect = luainfo["Reflect"].cast<bool>();

                m_tmpWheels->push_back(info);
                m_wheels.push_back(info2);
            }
        }
    }
    static void RegisterToLua(lua_State* L)
    {
        luabridge::getGlobalNamespace(L)
            .beginClass<Entity>("Entity")
            .addProperty("btVehicle", &Entity::GetComponent<BTVehicleComponent>)
            .endClass()
            .beginClass<BTVehicleComponent>("BTVehicle")
            .addFunction("AddWheel", &AddWheel)
            .addFunction("ApplyEngineForce", &ApplyEngineForce)
            .addFunction("SetBrake", &SetBrake)
            .addFunction("SetSteering", &SetSteering)
            .addFunction("GetSteering", &GetSteering)
            .addProperty("speed", &GetSpeed)
            .endClass()
            ;
    }
    ~BTVehicleComponent() { delete m_vehicle; m_wheels.clear(); }
    
    void CreateVehicle(btVehicleRaycaster* raycaster, btRigidBody* chassis)
    {
        if (m_vehicle) delete m_vehicle;
        m_vehicle = new btRaycastVehicle(m_tuning, chassis, raycaster);
        chassis->setActivationState(DISABLE_DEACTIVATION);

        for (unsigned i = 0; i < m_tmpWheels->size(); ++i)
        {
            TmpWheelInfo& tw = m_tmpWheels->at(i);
            NewWheel(tw.m_connectionPoint, tw.m_wheelDirection, tw.m_wheelAxle, tw.m_suspensionRestLength, tw.m_wheelRadius, tw.m_isFrontWheel);
        }
        delete m_tmpWheels; m_tmpWheels = NULL;
    }
    btRaycastVehicle* GetVehicle() { return m_vehicle; }

    void AddWheel(const glm::vec3 &connectionPoint, const glm::vec3 &wheelDirection, const glm::vec3 &wheelAxle, float suspensionRestLength, float wheelRadius, bool isFrontWheel)
    {
        if (m_vehicle)
            NewWheel(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, isFrontWheel);
        else
            m_tmpWheels->push_back(TmpWheelInfo(connectionPoint, wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, isFrontWheel));
        m_wheels.push_back(WheelInfo());
    }

    int GetWheelsNum() { return m_vehicle->getNumWheels(); }
    void ApplyEngineForce(int wheelIndex, float force) { m_vehicle->applyEngineForce(force, wheelIndex); }
    void SetBrake(int wheelIndex, float force) { m_vehicle->setBrake(force, wheelIndex); }
    void SetSteering(int wheelIndex, float steering) { m_vehicle->setSteeringValue(steering, wheelIndex); }
    float GetSteering(int wheelIndex) { return m_vehicle->getSteeringValue(wheelIndex); }

    void SetWheelEntity(int wheelIndex, Entity* entity, bool reflect = false)
    {
        if (!entity->HasComponent(TRANSFORM_COMPONENT)) entity = NULL;
        m_wheels[wheelIndex].entity = entity;
        m_wheels[wheelIndex].reflect = reflect;
    }
    Entity* GetWheelEntity(int wheelIndex) { return m_wheels[wheelIndex].entity; }
    bool IsWheelReflected(int wheelIndex) { return m_wheels[wheelIndex].reflect; }

    float GetSpeed() const { return m_vehicle->getCurrentSpeedKmHour(); }

protected:
    struct WheelInfo
    {
        WheelInfo() : entity(NULL), reflect(false) {}
        Entity* entity;
        bool reflect;
    };

    struct TmpWheelInfo
    {
        TmpWheelInfo() {}
        TmpWheelInfo(const glm::vec3 &connectionPoint, const glm::vec3 &wheelDirection, const glm::vec3 &wheelAxle, float suspensionRestLength, float wheelRadius, bool isFrontWheel)
        : m_connectionPoint(connectionPoint), m_wheelDirection(wheelDirection), m_wheelAxle(wheelAxle), m_suspensionRestLength(suspensionRestLength),
        m_wheelRadius(wheelRadius), m_isFrontWheel(isFrontWheel)
        {}
        glm::vec3 m_connectionPoint, m_wheelDirection, m_wheelAxle;
        float m_suspensionRestLength, m_wheelRadius;
        bool m_isFrontWheel;
        /*suspensionStiffmnes...*/
    };
    std::vector<TmpWheelInfo>* m_tmpWheels;      // For storage before adding vehicle

    btRaycastVehicle::btVehicleTuning m_tuning;
    btRaycastVehicle* m_vehicle;
    std::vector<WheelInfo> m_wheels;

    void NewWheel(const glm::vec3 &connectionPoint, const glm::vec3 &wheelDirection, const glm::vec3 &wheelAxle, float suspensionRestLength, float wheelRadius, bool isFrontWheel)
    {
        btWheelInfo& wheel = m_vehicle->addWheel(BTG(connectionPoint), BTG(wheelDirection), BTG(wheelAxle), suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
        wheel.m_suspensionStiffness = 50.0f;// 20.0f;
        wheel.m_wheelsDampingRelaxation = 4.4f;// 2.3f;
        wheel.m_wheelsDampingCompression = 2.3f;// 4.4f;
        wheel.m_frictionSlip = 10000.0f; //BT_LARGE_FLOAT;
        wheel.m_rollInfluence = 0.0003f;
    };
};

class BTVehicleSystem : public SystemBase<BTVEHICLE_COMPONENT, BTRIGIDBODY_COMPONENT>
{
public:
    BTVehicleSystem(BTPhysicsSystem* physicsSystem) : m_physicsSystem(physicsSystem) {}
    void Init() { m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_physicsSystem->GetWorld()); }
    void CleanUp() { delete m_vehicleRayCaster; }

    btVehicleRaycaster* GetDefaultRayCaster() { return m_vehicleRayCaster; }

    void InitEntity(Entity* entity)
    {
        BTVehicleComponent* vehicle = entity->GetComponent<BTVehicleComponent>();
        if (!vehicle->GetVehicle())  
            vehicle->CreateVehicle(m_vehicleRayCaster, entity->GetComponent<BTRigidBodyComponent>()->GetRigidBody());
        m_physicsSystem->GetWorld()->addAction(vehicle->GetVehicle());
        vehicle->GetVehicle()->resetSuspension();
    }
    void DeleteEntity(Entity* entity)
    {
        BTVehicleComponent* vehicle = entity->GetComponent<BTVehicleComponent>();
        m_physicsSystem->GetWorld()->removeAction(vehicle->GetVehicle());
    }

    void Update(double deltaTime)
    {
        for (unsigned i = 0; i < m_entities.size(); ++i)
        {
            Entity * entity = m_entities[i];
            BTVehicleComponent* vehicle = entity->GetComponent<BTVehicleComponent>(); 

            for (int j = 0; j < 4; ++j)
            {
                Entity* wheel = vehicle->GetWheelEntity(j);
                if (wheel)
                {
                    glm::mat4 trans;
                    vehicle->GetVehicle()->updateWheelTransform(j, true);
                    vehicle->GetVehicle()->getWheelInfo(j).m_worldTransform.getOpenGLMatrix(glm::value_ptr(trans));
                    if (vehicle->IsWheelReflected(j))
                        trans = trans * glm::rotate(glm::mat4(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    wheel->GetComponent<TransformComponent>()->SetWorldTransform(trans);
                }
            }
        }
    }

protected:
    BTPhysicsSystem * m_physicsSystem;
    btVehicleRaycaster* m_vehicleRayCaster;
};