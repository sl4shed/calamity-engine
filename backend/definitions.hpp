#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <string>
#include <vector>
#include <string>

struct Vector2D {
    float x;
    float y;
};

struct Component {
    virtual ~Component() = default;
    void update() {};
};

struct Transform : public Component {
    Vector2D position;
    Vector2D scale;
    Vector2D origin;
    float angle;
};

struct Texture {
    void* handle;
    int width;
    int height;
    std::string path;
};

class Sprite : public Component {
    public:
        Texture texture;
        Transform source_transform;
        bool visible;
        int z_index;
};

#endif
