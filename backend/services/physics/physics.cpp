#include "physics.hpp"
#include "../services.hpp"
#include "../../core/node/node.hpp"
#include "../../utils/logger.hpp"

void Shape::applyMaterial(Material mat)
{
    this->material = mat;
    shapeDef.density = mat.density;
    shapeDef.material.friction = mat.friction;
    shapeDef.material.restitution = mat.restitution;
    shapeDef.material.rollingResistance = mat.rollingResistance;
    shapeDef.material.tangentSpeed = mat.tangentSpeed;
}

Physics::Physics(Vector2 gravity) {
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)(gravity);
    worldId = b2CreateWorld(&worldDef);
}

void Physics::physicsUpdate(float timeStep) {
    b2World_Step(worldId, timeStep, subSteps);
}

Physics::~Physics() {
    Logger::debug("Destroying world");
    b2DestroyWorld(worldId);
}

Polygon::operator b2Polygon() const {
    b2Polygon polygon;
    polygon.centroid = (b2Vec2)centroid;
    polygon.count = count;
    for (int i = 0; i < count; i++) {
        polygon.normals[i] = (b2Vec2)normals[i];
        polygon.vertices[i] = (b2Vec2)vertices[i];
    }
    polygon.radius = radius;
    return polygon;
}

Polygon::Polygon(const b2Polygon &p) {
    centroid = Vector2(p.centroid);
    count = p.count;
    radius = p.radius;
    for (int i = 0; i < count; i++) {
        normals[i] = Vector2(p.normals[i]);
        vertices[i] = Vector2(p.vertices[i]);
    }
}

Polygon::Polygon() {
    centroid = {0, 0};
    count = 0;
    radius = 0;
    for (int i = 0; i < B2_MAX_POLYGON_VERTICES; i++) {
        normals[i] = {0, 0};
        vertices[i] = {0, 0};
    }
}

BoxShape::BoxShape(Vector2 size, Vector2 center) {
    this->size = size;
    this->origin = center;

    Vector2 calculatedCenter = origin * size;
    b2Rot rotation = b2Rot_identity;

    b2Polygon poly = b2MakeOffsetBox(size.x / 2 * Physics::scale, size.y / 2 * Physics::scale, (b2Vec2)(calculatedCenter * Physics::scale), rotation);
    b2Polygon polyUnscaled = b2MakeOffsetBox(size.x / 2, size.y / 2, (b2Vec2)calculatedCenter, rotation);

    this->polygon = (Polygon)polyUnscaled;
    this->shapeDef = b2DefaultShapeDef();
    this->scaledPolygon = (Polygon)poly;
}

////////////////////
// rigid body //////
///////////////////

RigidBody::RigidBody() {}
RigidBody::RigidBody(std::shared_ptr<Shape> shape) {
    this->shape = shape;

    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.linearDamping = 0.1f;
    bodyDef.angularDamping = 0.1f;

    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);

    b2Polygon poly = shape->scaledPolygon;
    b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
}

void RigidBody::physicsUpdate() {
    Node* node = getNode();

    // send box2d pos to world pos
    Vector2 worldPos = Vector2(b2Body_GetPosition(bodyId)) / Physics::scale;
    float worldAngle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));

    if (node->parent) {
        Transform parentInverse = node->parent->globalTransform.inverse();
        node->transform.position = parentInverse.transformation * (worldPos - node->parent->globalTransform.position);
        node->transform.setAngle(worldAngle - node->parent->globalTransform.getAngle());
    } else {
        node->transform.position = worldPos;
        node->transform.setAngle(worldAngle);
    }

    storedTransform.position = Vector2(b2Body_GetPosition(bodyId));
    storedTransform.setAngle(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));
}

void RigidBody::setPosition(Vector2 pos)
{
    b2Body_SetTransform(bodyId, (b2Vec2)pos, b2Body_GetRotation(bodyId));
    // this is supposed to null velocity but i think it does that anyway?
}

void RigidBody::setAngle(float angle)
{
    b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(getNode()->globalTransform.getAngle()), sin(getNode()->globalTransform.getAngle())});
    // this is supposed to null velocity but idk
}

void RigidBody::setLinearVelocity(Vector2 vel)
{
    b2Body_SetLinearVelocity(bodyId, vel);
}

void RigidBody::applyForce(Vector2 force)
{
    b2Body_ApplyForce(bodyId, force, shape->origin, true);
    // i guess, always wake the body? that makes the most sense
}

void RigidBody::applyImpulse(Vector2 impulse)
{
    b2Body_ApplyLinearImpulse(bodyId, impulse, shape->origin, true);
}

void RigidBody::initialize() {
    auto tr = this->getNode()->globalTransform;
    b2Body_SetTransform(bodyId, tr.position * Physics::scale, {cos(tr.getAngle()), sin(tr.getAngle())});
}

RigidBody::~RigidBody() {
    b2DestroyBody(bodyId);
}

/////////////////////////////
// static body             //
/////////////////////////////

StaticBody::StaticBody() {}
StaticBody::StaticBody(std::shared_ptr<Shape> shape) {
    this->shape = shape;

    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_staticBody;
    bodyDef.linearDamping = 0.1f;
    bodyDef.angularDamping = 0.1f;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);

    b2Polygon poly = shape->scaledPolygon;
    b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
}

void StaticBody::physicsUpdate() {
    storedTransform.position = Vector2(b2Body_GetPosition(bodyId));
    storedTransform.setAngle(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));
}

void StaticBody::setPosition(Vector2 pos)
{
    b2Body_SetTransform(bodyId, (b2Vec2)pos, b2Body_GetRotation(bodyId));
    // this is supposed to null velocity but i think it does that anyway?
}

void StaticBody::setAngle(float angle)
{
    b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(getNode()->globalTransform.getAngle()), sin(getNode()->globalTransform.getAngle())});
    // this is supposed to null velocity but idk
}


void StaticBody::initialize() {
    auto tr = this->getNode()->globalTransform;
    b2Body_SetTransform(bodyId, tr.position * Physics::scale, {cos(tr.getAngle()), sin(tr.getAngle())});
}

StaticBody::~StaticBody() {
    b2DestroyBody(bodyId);
}