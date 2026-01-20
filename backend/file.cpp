#include "file.hpp"
#include "json.hpp"
#include <iostream>
#include <iomanip>
using json = nlohmann::json;

// this is a separate library because i will be implementing the save system here too, so it's open for expansion
// right now it only serves the purpose of loading json node trees

std::string readFileText(std::string path)
{
    const char *cpath = path.c_str();
    SDL_RWops *rw = SDL_RWFromFile(cpath, "rb");
    if (!rw)
        return {};
    Sint64 size = SDL_RWsize(rw);
    if (size <= 0)
    {
        SDL_RWclose(rw);
        return {};
    }
    std::string out;
    out.resize((size_t)size);
    size_t read = SDL_RWread(rw, out.data(), 1, (size_t)size);
    SDL_RWclose(rw);
    if (read != (size_t)size)
        return {}; // handle partial reads if needed
    return out;
}

std::string exportNodeTree(Engine *engine)
{
    json tree;
    tree[0] = "test";
    std::cout << std::setw(4) << tree << std::endl;
    return std::string("");
}

void loadNodeTree(Engine *engine, std::string jsonText)
{
    // nothing yet
}