#pragma once
#include "../node/components.hpp"
#include "../definitions.hpp"
#include "../../services/graphics.hpp"

/**
 * # Label
 * The Label component renders text to the screen.
 * You can construct it with or without a font, in which case a default font will be used.
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
 * ## Properties and usages
 * 
 * You can change the Label components origin. I'm really tired of saying what origin does over and over again...
 * ```cpp
 * label->origin = {0.0f, 0.5f};
 * ```
 *
 * The Label component also exposes a `wrap` attribute. This defines if the text is rendered using text wrap or not.
 * ```cpp
 * label->wrap = false; // Disable text wrapping
 * ```
 * 
 * You can also make Labels render using screen space positioning. This can be used for UI elements and things like that:
 * ```cpp
 * label->screenSpace = true; // Now, the position of the label will directly translate to screen coordinates!
 * ```
 * 
 * You can also...
 * ```cpp
 * label->setColor(Color(255, 0, 0)); // Set the labels color!
 * label->setDirection(FontDirection::LTR); // And set the text direction.
 * ```
 * 
 * Make sure to also check out the [text example](https://calamity.sl4shed.xyz/example-text)!
 */
class Label : public Component {
public:
    Label(const std::string& text, Font *font);
    Label(const std::string& text);
    ~Label();

    Vector2 size;
    Vector2 origin;

    void update(float dt);
    Font* font;
    bool visible = true;
    bool wrap = true; // text wrap. yes or no?
    bool screenSpace = false;

    Label* setText(const std::string& text);
    std::string getText() const;
    TTF_Text* getHandle() const;

    Label* setColor(Color color);
    Color getColor() const;
    Label* setDirection(FontDirection direction);
    Label* setWrapWidth(int width); // meant for internal use
    int getWrapWidth() const;
    void rebuildTexture();
    SDL_Texture *getTexture() const;

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
    SDL_Texture *texture = nullptr;
    Vector2 prevSize;
};