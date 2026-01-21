#include "components.hpp"
#include "node.hpp"
#include "engine.hpp"

Node *Component::getNode()
{
    return this->node;
}

void Component::setNode(Node *n)
{
    // for now its just this, but i can add checks later to only run this once or maybe i should find a better solution idk
    this->node = n;
}

void Camera::setActive(Engine &engine)
{
    engine.setActiveCamera(this);
    this->active = true;
}