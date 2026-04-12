#include "physics.hpp"
#include "../services.hpp"
#include "../../core/node/node.hpp"
#include "../../utils/logger.hpp"

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
    this->center = center;

    Vector2 calculatedCenter = center * size;
    b2Rot rotation = b2Rot();
    
    // scaled version for Box2D
    b2Polygon poly = b2MakeOffsetBox(size.x / 2 * Physics::scale, size.y / 2 * Physics::scale, (b2Vec2)(calculatedCenter * Physics::scale), rotation);
    
    // unscaled version for rendering
    b2Polygon polyUnscaled = b2MakeOffsetBox(size.x / 2, size.y / 2, (b2Vec2)calculatedCenter, rotation);
    this->polygon = (Polygon)polyUnscaled;
    
    this->shapeDef = b2DefaultShapeDef();
    
    // store scaled separately for body creation
    this->scaledPolygon = (Polygon)poly;
}

StaticBody::StaticBody() {}
StaticBody::StaticBody(std::shared_ptr<Shape> shape) {
    this->shape = shape;

    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_staticBody;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
    b2Polygon poly = shape->scaledPolygon;
    b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
}

RigidBody::RigidBody() {}
RigidBody::RigidBody(std::shared_ptr<Shape> shape) {
    this->shape = shape;

    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyId = b2CreateBody(Services::physics()->worldId, &bodyDef);
    b2Polygon poly = shape->scaledPolygon;
    b2CreatePolygonShape(bodyId, &shape->shapeDef, &poly);
}

void RigidBody::physicsUpdate() {
    if (getNode()->transform.position != prevPosition) {
        b2Body_SetTransform(bodyId, (b2Vec2)(getNode()->transform.position * Physics::scale), b2Body_GetRotation(bodyId));
    }
    prevPosition = Vector2(b2Body_GetPosition(bodyId)) / Physics::scale;

    if (getNode()->transform.getAngle() != prevAngle) {
        b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(getNode()->transform.getAngle()), sin(getNode()->transform.getAngle())});
    }
    prevAngle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));

    // read back from box2d as normal
    getNode()->transform.position = Vector2(b2Body_GetPosition(bodyId)) / Physics::scale;
    getNode()->transform.setAngle(b2Rot_GetAngle(b2Body_GetRotation(bodyId)));

    // debug
    ShapeSprite *polySprite = getNode()->getComponent<ShapeSprite>();
    if (polySprite) {
        polySprite->shape = shape->polygon;
    }
}

void StaticBody::physicsUpdate() {
    if (getNode()->transform.position != prevPosition) {
        b2Body_SetTransform(bodyId, (b2Vec2)(getNode()->transform.position * Physics::scale), b2Body_GetRotation(bodyId));
    }
    prevPosition = Vector2(b2Body_GetPosition(bodyId)) / Physics::scale;

    if (getNode()->transform.getAngle() != prevAngle) {
        b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), {cos(getNode()->transform.getAngle()), sin(getNode()->transform.getAngle())});
    }
    prevAngle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));
}

void StaticBody::initialize() {
    b2Body_SetTransform(bodyId, (b2Vec2)(getNode()->transform.position * Physics::scale), 
        {cos(getNode()->transform.getAngle()), sin(getNode()->transform.getAngle())});
}

void RigidBody::initialize() {
    b2Body_SetTransform(bodyId, (b2Vec2)(getNode()->transform.position * Physics::scale),
        {cos(getNode()->transform.getAngle()), sin(getNode()->transform.getAngle())});
}

RigidBody::~RigidBody() {
    Logger::debug("destroying rigidbody");
    b2DestroyBody(bodyId);
}

StaticBody::~StaticBody() {
    Logger::debug("destroying staticbody");
    b2DestroyBody(bodyId);
}