#include "physics.hpp"

Physics::Physics(Vector2 gravity) {
    
}

void Physics::initialize() {
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;

    worldId = b2CreateWorld(&worldDef);
}