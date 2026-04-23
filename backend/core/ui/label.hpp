#pragma once
#include "../node/components.hpp"
#include "../definitions.hpp"
#include "../../services/graphics.hpp"

/**
 * # Label
 * A component which renders text (wow, amazing. I know.)
 * You can construct it with or without a font, in which case a default font will be used.
 *
 * Also dictates a size and origin, similar to a Sprite.
 *
 * Example Usage:
 * ```cpp
 * std::shared_ptr<Node> node = std::make_shared<Node>();
 * std::shared_ptr<Label> label = std::make_shared<Label>("Text goes here");
 * label->size = {100, 500};
 * node->transform.position = {20, 20};
 * node->addComponent(label);
 * ```
 *
 * Make sure to also check out the text example in the examples folder!
 */
class Label : public Component {
public:
    Label(std::string text, Font *font);
    Label(std::string text);
    ~Label();

    Vector2 size;
    Vector2 origin;

    void update(float dt);
    Font* font;
    bool visible = true;
    bool wrap = true; // text wrap. yes or no?

    Label* setText(std::string text);
    std::string getText() const;
    TTF_Text* getHandle() const;

    Label* setColor(Color color);
    Color getColor();
    Label* setDirection(FontDirection direction);
    Label* setWrapWidth(int width);
    int getWrapWidth();
    void rebuildTexture();
    SDL_Texture *getTexture();

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(size), CEREAL_NVP(origin), CEREAL_NVP(font), CEREAL_NVP(visible), CEREAL_NVP(wrap), CEREAL_NVP(color), CEREAL_NVP(direction), CEREAL_NVP(wrapWidth), CEREAL_NVP(text));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(size), CEREAL_NVP(origin), CEREAL_NVP(font), CEREAL_NVP(visible), CEREAL_NVP(wrap), CEREAL_NVP(color), CEREAL_NVP(direction), CEREAL_NVP(wrapWidth), CEREAL_NVP(text));
        TTF_CreateText(Services::graphics()->getTextEngine(), font->getHandle(), text.c_str(), text.size());
        TTF_SetTextColor(handle, color.r, color.g, color.b, color.a);
        TTF_SetTextDirection(handle, (TTF_Direction)direction);
        TTF_SetTextWrapWidth(handle, wrapWidth);
    }
private:
    std::string text = "";
    Color color = Color::WHITE;
    FontDirection direction = FontDirection::LTR;
    int wrapWidth = -1;
    bool dirty = true;

    TTF_Text* handle; // TTF_Text*
    SDL_Texture *texture;
    Vector2 prevSize;
};