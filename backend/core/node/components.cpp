#include "components.hpp"
#include "node.hpp"
#include "../../services/engine.hpp"
#include "../../services/services.hpp"
#include "../../utils/logger.hpp"
#include <cereal/archives/json.hpp>

Node* Component::getNode()
{
    return this->node;
}

void Component::setNode(Node* n)
{
    // for now its just this, but i can add checks later to only run this once or maybe i should find a better solution idk
    this->node = n;
}

void Camera::setActive()
{
    Services::engine()->setActiveCamera(this);
    this->active = true;
}

void Camera::initialize()
{
    if (this->active)
    {
        setActive();
    }
}

PolygonSprite::PolygonSprite() {}
PolygonSprite::PolygonSprite(Polygon shape)
{
    this->shape = shape;
}