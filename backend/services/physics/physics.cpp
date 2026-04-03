#include "physics.hpp"

Physics::Physics(Vector2 gravity) {
    
}

void Physics::initialize() {
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2)gravity;

    worldId = b2CreateWorld(&worldDef);
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

explicit Polygon::Polygon(const b2Polygon &p) {
    centroid = Vector2(p.centroid);
    count = p.count;
    radius = p.radius;
    for (int i = 0; i < count; i++) {
        normals[i] = Vector2(p.normals[i]);
        vertices[i] = Vector2(p.vertices[i]);
    }
}

BoxShape::BoxShape(Vector2 size, Vector2 center) {
    this->size = size;
    this->center = center;

    Vector2 calculatedCenter = center * size;
    b2Rot rotation = {cos(0), sin(0)};
    b2Polygon poly = b2MakeOffsetBox(size.x / 2, size.y / 2, (b2Vec2)calculatedCenter, rotation);
    this->polygon = (Polygon)poly;
    this->shapeDef = b2DefaultShapeDef();
}

StaticBody::StaticBody(std::shared_ptr<Shape> shape) {
    this->shape = shape;

    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2BodyType::b2_staticBody;
    b2BodyId groundId = b2CreateBody(Services::physics()->worldId, &bodyDef);
    b2CreatePolygonShape(groundId, &shape->shapeDef, &(b2Polygon)shape->polygon);
}