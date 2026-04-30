#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "../definitions.hpp"

/**
 * Mirrors the [TTF_HintingFlags](https://wiki.libsdl.org/SDL3_ttf/TTF_HintingFlags) enum.
 * Defines font hinting.
 */
enum class FontHinting {
    INVALID = -1,
    NORMAL,
    LIGHT,
    MONO,
    NONE,
    LIGHT_SUBPIXEL
};

/**
 * Mirrors the [TTF_FontStyleFlags](https://wiki.libsdl.org/SDL3_ttf/TTF_FontStyleFlags) enum.
 * Defines the font style used (bold, italic, underline, etc...).
 */
enum class FontStyle {
    NORMAL = 0x00,
    BOLD = 0x01,
    ITALIC = 0x02,
    UNDERLINE = 0x04,
    STRIKETHROUGH = 0x08
};

/**
 * Mirrors the [TTF_Direction](https://wiki.libsdl.org/SDL3_ttf/TTF_Direction) enum.
 * Defines the direction of the text (left to right, right to left, etc...).
 */
enum class FontDirection {
    INVALID,
    LTR = 4,
    RTL,
    TTB,
    BTT
};

/**
 * Mirrors the [TTF_HorizontalAlignment](https://wiki.libsdl.org/SDL3_ttf/TTF_HorizontalAlignment) enum.
 * Defines the horizontal alignment of the font.
 */
enum class FontAlignment {
    INVALID = -1,
    LEFT,
    CENTER,
    RIGHT
};

/**
 * # Font
 * Defines a TrueType font and a myriad of properties.
 *
 * Example usage:
 * ```cpp
 * Font font = Font("path/to/font");
 * font.setKerning(true)->setLineSpacing(10)->setSize(30)->setStyle(FontStyle::UNDERLINE);
 * ```
 * 
 * ## Properties and usages
 * You can...
 * 
 * ```
 * font.setKerning(true); // Enable or disable kerning on the font
 * font.setHinting(FontHinting::MONO); // Change the hinting of the font
 * font.setLanguage("en-US"); // Set the language of the font
 * font.setLineSpacing(20); // Set the amount of spacing between lines
 * font.setOutline(true); // Enable/Disable an outline
 * font.setSDF(false); // DISABLE SDF. It's bad... please don't use it :(
 * font.setSize(40); // Set the font size
 * font.setStyle(FontStyle::UNDERLINE); // Change the font style!
 * font.setAlignment(FontAlignment::CENTER) // And finally, set the alingment of the font!
 * ```
 */
class Font {
public:
    Font();
    Font(const std::string& path);
    ~Font();
    std::string path; // File path of the font.
    int size = 30;

    Font* setKerning(bool enabled);
    Font* setHinting(FontHinting setting);
    Font* setLanguage(const std::string& language); // bcp47 code please
    Font* setLineSpacing(int spacing);
    Font* setOutline(int outline);
    Font* setSDF(bool enabled);
    Font* setSize(int ptSize);
    Font* setStyle(FontStyle style);
    Font* setAlignment(FontAlignment positioning);

    TTF_Font* getHandle() const;

    template <class Archive>
    void save(Archive &ar) const {
        ar(CEREAL_NVP(path), CEREAL_NVP(size), CEREAL_NVP(kerning), CEREAL_NVP(hinting), CEREAL_NVP(language), CEREAL_NVP(lineSpacing), CEREAL_NVP(outline), CEREAL_NVP(SDF), CEREAL_NVP(alignment), CEREAL_NVP(style), CEREAL_NVP(alignment));
    }

    template <class Archive>
    void load(Archive &ar) {
        ar(CEREAL_NVP(path), CEREAL_NVP(size), CEREAL_NVP(kerning), CEREAL_NVP(hinting), CEREAL_NVP(language), CEREAL_NVP(lineSpacing), CEREAL_NVP(outline), CEREAL_NVP(SDF), CEREAL_NVP(alignment), CEREAL_NVP(style), CEREAL_NVP(alignment));
        TTF_OpenFont(path.c_str(), size);
        //TTF_SetFontSize((TTF_Font*)handle, size);
        TTF_SetFontKerning((TTF_Font*)handle, kerning);
        TTF_SetFontHinting((TTF_Font*)handle, (TTF_HintingFlags)hinting);
        TTF_SetFontLanguage((TTF_Font*)handle, language.c_str());
        TTF_SetFontLineSkip((TTF_Font*)handle, lineSpacing);
        TTF_SetFontOutline((TTF_Font*)handle, outline);
        TTF_SetFontSDF((TTF_Font*)handle, SDF);
        TTF_SetFontStyle((TTF_Font*)handle, (TTF_FontStyleFlags)style);
        TTF_SetFontWrapAlignment((TTF_Font*)handle, (TTF_HorizontalAlignment)alignment);
    }
private:
    bool kerning = true;
    FontHinting hinting = FontHinting::NONE;
    std::string language = "en-US";
    int lineSpacing;
    int outline = 0;
    bool SDF = false;
    FontStyle style = FontStyle::NORMAL;
    FontAlignment alignment = FontAlignment::LEFT;

    TTF_Font* handle;
};