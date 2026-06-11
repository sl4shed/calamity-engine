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

class RaycastScript : public Script
{
    const int SPEED = 100;
    Node *me;

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
        if (result.hit)
        {
            Logger::debug("Raycast hit!!!");
            drawSegment(me->transform.position, result.point, Color::RED, me->getWindow().get());
        }
    }
};

CEREAL_REGISTER_TYPE(RaycastScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RaycastScript);