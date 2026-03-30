#pragma once
#include "../node/components.hpp"
#include "../definitions.hpp"

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
private:
    TTF_Text* handle; // TTF_Text*
    Vector2 prevSize;
};