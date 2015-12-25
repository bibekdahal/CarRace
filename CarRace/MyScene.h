#pragma once

#include "engine/Scene.h"

class MyScene : public Scene
{
public:
    MyScene(Window* window, Engine* engine, Graphics* graphics) : Scene(window, engine, graphics) {}
private:
    void Init();
};