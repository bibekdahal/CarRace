//#include "MyScene.h"
//#include "engine/ModelRenderSystem.h"
//#include "engine/CameraSystem.h"
//#include "graphics/StandardMaterials.h"
//#include "lua/LuaScriptSystem.h"
//
//#include "D:/bullet-2.82-r2704/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
//
//
//inline std::ostream& operator << (std::ostream& os, glm::vec3& v)
//{
//    os << v.x << " " << v.y << " " << v.z << std::endl;
//    return os;
//}
//
//void MyScene::Init()
//{
//    unsigned int followScript,  inputScript;
//
//    m_systems.Add(new CameraSystem(m_graphics));
//    m_systems.Add(new ModelRenderSystem);
//    unsigned int p = m_systems.Add(new BTPhysicsSystem);
//    m_systems.Add(new BTVehicleSystem((BTPhysicsSystem*)m_systems[p]));
//    //m_systems.Add(new MyInputSystem(m_window));
//    m_systems.Add(new LuaScriptSystem()); 
//    InitializeSystems();                        /// Always initialize systems before adding entities
//    
//    followScript = LuaScriptSystem::AddScript(m_window, "scripts/follow_script.lua");
//    inputScript = LuaScriptSystem::AddScript(m_window, "scripts/input_script.lua");
//    ////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Entity* entity = new Entity;
//    Model * model = new Model;
//    entity->AddComponent<ModelComponent>(Graphics::GetModelManager().Add(model));
//
//    glm::vec3 min, max;
//    model->LoadFromFile("car.dat", &min, &max);
//    //min += glm::vec3(0.0f, 0.2f, 0.0f);
//    glm::vec3 center = (min + max) / 2.0f;
//    glm::vec3 extents = (max - min) / 2.0f;
//    for (unsigned i = 0; i < model->GetMeshesSize(); ++i)
//    {
//        Mesh * mesh = Graphics::GetMeshManager()[model->GetMesh(i)];
//        mesh->SetOffset(glm::translate(glm::mat4(), -center));
//    }
//    entity->AddComponent<BTColliderComponent>(m_engine->AddItem<BTCollisionShape>("car.shp"));
//    
//    TransformComponent* trans = entity->AddComponent<TransformComponent>();
//    trans->SetPosition(glm::vec3(1.5f, -2.0f, -1.0f)); 
//    trans->SetRotation(glm::rotate(glm::quat(), -glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
//    entity->AddComponent<BTRigidBodyComponent>(250.0f);
//    BTVehicleComponent * vehicle = entity->AddComponent<BTVehicleComponent>();
//    entity->AddComponent<LuaScriptComponent>()->AddScripts(inputScript);
//
//
//
//    // Coloring green
//    for (unsigned i = 0; i < model->GetMaterialsSize(); ++i)
//    {
//        DiffuseMaterial* mat = (DiffuseMaterial*)Graphics::GetMaterialManager()[model->GetMaterial(i)];
//        if (mat->GetDiffuseColor().r > mat->GetDiffuseColor().g)    // check if red component is high
//            mat->SetDiffuseColor(mat->GetDiffuseColor() + glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
//    }
//    
//    /////////////////////// VEHICLE SETUP //////////////////////////
//
//    glm::vec3 wheeldir(0.0f, -1.0f, 0.0f), wheelaxle(0.0f, 0.0f, -1.0f);
//    float wheelWidth = 1.0f; float cheight = -extents[1]+0.7f; float wheelRadius = 0.7f;
//    glm::vec3 cpoint;
//
//    cpoint = glm::vec3(-extents[0] + 1.5f, cheight, -extents[2] + 0.3*wheelWidth);
//    vehicle->AddWheel(cpoint, wheeldir, wheelaxle, 0.6f, wheelRadius, true);
//    cpoint = glm::vec3(-extents[0] + 1.5f, cheight, extents[2] - 0.3*wheelWidth);
//    vehicle->AddWheel(cpoint, wheeldir, wheelaxle, 0.6f, wheelRadius, true);
//    cpoint = glm::vec3(extents[0] - 1.0f, cheight, -extents[2] + 0.3*wheelWidth);
//    vehicle->AddWheel(cpoint, wheeldir, wheelaxle, 0.6f, wheelRadius, false);
//    cpoint = glm::vec3(extents[0] - 1.0f, cheight, extents[2] - 0.3*wheelWidth);
//    vehicle->AddWheel(cpoint, wheeldir, wheelaxle, 0.6f, wheelRadius, false);
//    
//    ////////////////////////////////////////////////////////////////
//
//    Model * wmodel = new Model;
//    wmodel->LoadFromFile("wheel.dat");
//    for (unsigned i = 0; i < wmodel->GetMeshesSize(); ++i)
//    {
//        Mesh * mesh = Graphics::GetMeshManager()[wmodel->GetMesh(i)];
//        mesh->SetOffset(glm::rotate(glm::mat4(), -glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
//    }
//
//    Entity* wheel = new Entity;
//    TransformComponent* twheel = wheel->AddComponent<TransformComponent>();
//    twheel->SetParent(entity->GetComponent<TransformComponent>());
//    unsigned wmid = Graphics::GetModelManager().Add(wmodel);
//    wheel->AddComponent<ModelComponent>(wmid);
//    vehicle->SetWheelEntity(0, wheel);
//    AddEntity(wheel);
//    
//    wheel = new Entity;
//    twheel = wheel->AddComponent<TransformComponent>();
//    twheel->SetParent(entity->GetComponent<TransformComponent>());
//    wheel->AddComponent<ModelComponent>(wmid);
//    AddEntity(wheel);
//    vehicle->SetWheelEntity(1, wheel, true);
//
//    wheel = new Entity;
//    twheel = wheel->AddComponent<TransformComponent>();;
//    twheel->SetParent(entity->GetComponent<TransformComponent>());
//    wheel->AddComponent<ModelComponent>(wmid);
//    AddEntity(wheel);
//    vehicle->SetWheelEntity(2, wheel);
//
//    wheel = new Entity;
//    twheel = wheel->AddComponent<TransformComponent>();
//    twheel->SetParent(entity->GetComponent<TransformComponent>());
//    wheel->AddComponent<ModelComponent>(wmid);
//    AddEntity(wheel);
//    vehicle->SetWheelEntity(3, wheel, true);
//    
//    AddEntity(entity);
//    ////////////////////////////////////////////////////////////////
//
//    float scale = 100.0f;
//    Entity* entity2 = new Entity;
//    TransformComponent* tcomp2 = entity2->AddComponent<TransformComponent>();
//    Model* newModel = new Model;
//    newModel->LoadFromFile("race-track.dat");
//    entity2->AddComponent<ModelComponent>(Graphics::GetModelManager().Add(newModel))->SetScale(glm::vec3(scale));
//
//    auto cshp = entity2->AddComponent<BTColliderComponent>(m_engine->AddItem<BTCollisionShape>("race-track.shp"));
//    cshp->GetCollisionShape()->setLocalScaling(btVector3(scale, scale, scale));
//    tcomp2->SetPosition(glm::vec3(-30.0f, -22.0f, -6.8f));
//    tcomp2->SetRotation(glm::rotate(glm::quat(), -glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::quat(), -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
//    entity2->AddComponent<BTRigidBodyComponent>(0.0f);
//
//    AddEntity(entity2);
//
//    
//    ///////////////// Camera /////////////////////
//
//    Entity* cameraEntity = new Entity;
//    CameraComponent* camera = cameraEntity->AddComponent<CameraComponent>();
//    TransformComponent* camtrans = cameraEntity->AddComponent<TransformComponent>();
//    LuaScriptComponent* sc1;
//    (sc1 = cameraEntity->AddComponent<LuaScriptComponent>())->AddScripts(followScript);
//    AddEntity(cameraEntity);
//    camera->SetViewport(Rect(0, 0.5f, 1.0f, 0.5f));
//    sc1->SetData("InverseCamera", false);
//    sc1->SetData("Target", entity->GetComponent<TransformComponent>());
//
//    Entity* cameraEntity2 = new Entity;
//    CameraComponent* camera2 = cameraEntity2->AddComponent<CameraComponent>();
//    TransformComponent* camtrans2 = cameraEntity2->AddComponent<TransformComponent>();
//    LuaScriptComponent* sc2;
//    (sc2 = cameraEntity2->AddComponent<LuaScriptComponent>())->AddScripts(followScript);
//    AddEntity(cameraEntity2);
//    camera2->SetViewport(Rect(0, 0, 1.0f, 0.5f));
//
//    sc2->SetData("InverseCamera", true);
//    sc2->SetData("Target", entity->GetComponent<TransformComponent>());
//    /////////////////////////////////////////////
//}