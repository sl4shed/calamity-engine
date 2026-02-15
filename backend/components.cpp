#include "components.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "services.hpp"
#include <cereal/archives/json.hpp>

Node *Component::getNode()
{
    return this->node;
}

void Component::setNode(Node *n)
{
    // for now its just this, but i can add checks later to only run this once or maybe i should find a better solution idk
    this->node = n;
}

void Camera::setActive()
{
    Services::engine()->setActiveCamera(this);
    this->active = true;
}

// Explicit template instantiation for Camera::load
// template void Camera::load<cereal::JSONInputArchive>(cereal::JSONInputArchive &ar);
// template void Camera::load<cereal::JSONOutputArchive>(cereal::JSONOutputArchive &ar);