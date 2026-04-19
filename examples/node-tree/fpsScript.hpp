#pragma once
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include <fmt/format.h>

#include "backend/core/node/components.hpp"
#include "backend/core/ui/label.hpp"

class FpsScript : public Script
{
    Label* label = nullptr;
    float elapsed = 0.0f;
    int frames = 0;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        label = getNode()->getComponent<Label>().get();
    }

    void update(float deltaTime)
    {
        elapsed += deltaTime;
        frames++;

        if (elapsed >= 0.5f) {
            float fps = frames / elapsed;
            label->setText(fmt::format("FPS: {:.0f}", fps));
            elapsed = 0.0f;
            frames = 0;
        }
    }
};

CEREAL_REGISTER_TYPE(FpsScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, FpsScript);