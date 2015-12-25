#include "engine/StandardSystems.h"
#include "BTPhysics/BTVehicle.h"

class MyInputSystem : public SystemBase<BTVEHICLE_COMPONENT, PLAYER_COMPONENT>
{
public:
    MyInputSystem(Window* window) : m_window(window) {}
    void Update(double deltaTime)
    {
        float steeringIncrement = (float)deltaTime*0.13f;
        const float steeringClamp = 0.25f;

        if (m_window->CheckKey(GLFW_KEY_W)) m_engineForce = 500;
        else if (m_window->CheckKey(GLFW_KEY_S)) m_engineForce = -500;
        else m_engineForce = 0;

        m_brakingForce = 0.0f;

        if (m_window->CheckKey(GLFW_KEY_A))
        {
            m_steering += steeringIncrement;
            if (m_steering > steeringClamp)
                m_steering = steeringClamp;
        }
        if (m_window->CheckKey(GLFW_KEY_D))
        {
            m_steering -= steeringIncrement;
            if (m_steering < -steeringClamp)
                m_steering = -steeringClamp;
        }
        if (m_window->CheckKey(GLFW_KEY_SPACE)) m_brakingForce = 100;
        else m_brakingForce = 0;
        
        for (unsigned i = 0; i < m_entities.size(); ++i)
        {
            Entity* entity = m_entities[i];
            BTVehicleComponent* vehicle = entity->GetComponent<BTVehicleComponent>();
            float eforce = m_engineForce;
            if (fabs(vehicle->GetVehicle()->getCurrentSpeedKmHour()) > 80) eforce = 0;
            vehicle->ApplyEngineForce(2, eforce);
            vehicle->SetBrake(2, m_brakingForce);
            vehicle->ApplyEngineForce(3, eforce);
            vehicle->SetBrake(3, m_brakingForce);

            vehicle->SetSteering(0, m_steering);
            vehicle->SetSteering(1, m_steering);
        }
    }

private:
    Window* m_window;
    float m_engineForce, m_brakingForce, m_steering;
};