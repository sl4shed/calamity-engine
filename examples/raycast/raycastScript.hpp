#pragma once
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include <fmt/format.h>

#include "backend/utils/utils.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/services/physics/definitions.hpp"
#include "backend/services/services.hpp"
#include "backend/services/input/input.hpp"
#include "backend/core/node/components.hpp"
#include "backend/core/ui/label.hpp"
#include "backend/core/ui/definitions.hpp"

class RaycastScript : public Script
{
    const int SPEED = 100;
    Node *me;

    RaycastResult currentResult;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        me = getNode();
    }

    void update(float deltaTime)
    {
        float move = Services::input()->getAxis("left", "right");
        me->transform.rotate(move * SPEED * deltaTime);

        Raycast ray = Raycast();
        ray.transform = me->transform;
        RaycastResult result = ray.calculate();
        currentResult = result;
    }

    void render(std::shared_ptr<Window> window) {
        if(!currentResult.hit) return;
        auto cam = window->getActiveCamera();
        const Vector2 screenSize = window->dimensions.size;
        const auto originOffset = Vector2{screenSize.x * cam->origin.x, screenSize.y * cam->origin.y};
        const Transform cameraTransform = cam->getCameraTransform();
        auto cameraInverse = cameraTransform.inverse();

        auto point1 = toScreen(me->transform.position, cameraTransform, cameraInverse, originOffset, false);
        auto point2 = toScreen(currentResult.point, cameraTransform, cameraInverse, originOffset, false);

        drawSegment(point1, point2, Color::RED, window.get());
    }
};

CEREAL_REGISTER_TYPE(RaycastScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RaycastScript);