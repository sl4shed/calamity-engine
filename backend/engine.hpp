#pragma once
#include "definitions.hpp"
#include "node.hpp"
#include "components.hpp"

class Engine
{
public:
    Engine();
    ~Engine();
    Node root;
    void update();
    void render(Graphics &graphics);
};