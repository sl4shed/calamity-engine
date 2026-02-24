#include "file.hpp"
#include <iostream>
#include <iomanip>
#include "logger.hpp"
#include "services.hpp"

// std::string readFileText(std::string path)
// {
//     // const char *cpath = path.c_str();
//     // SDL_RWops *rw = SDL_RWFromFile(cpath, "rb");
//     // if (!rw)
//     //     return {};
//     // Sint64 size = SDL_RWsize(rw);
//     // if (size <= 0)
//     // {
//     //     SDL_RWclose(rw);
//     //     return {};
//     // }
//     // std::string out;
//     // out.resize((size_t)size);
//     // size_t read = SDL_RWread(rw, out.data(), 1, (size_t)size);
//     // SDL_RWclose(rw);
//     // if (read != (size_t)size)
//     //     return {}; // handle partial reads if needed
//     // return out;
//     return "peepeepoopoo";
// }

std::string exportNodeTree(std::shared_ptr<Node> node)
{
    std::stringstream o;
    cereal::JSONOutputArchive archive(o);
    archive(node);
    return o.str();
}

/**
 * This function is specifically for exporting the root node of the engine.
 */
std::string exportNodeTree()
{
    std::stringstream o;
    cereal::JSONOutputArchive archive(o);
    archive(Services::engine()->root);
    return o.str();
}

void loadNodeTree(std::shared_ptr<Node> parent, std::string jsonText)
{
    std::stringstream i(jsonText);
    cereal::JSONInputArchive archive(i);
    std::shared_ptr<Node> node = std::make_shared<Node>();
    archive(node);
    parent->addChild(node);
}

/**
 * This function is specifically for loading the root node of the engine.
 */
void loadNodeTree(std::string jsonText)
{
    std::stringstream i(jsonText);
    cereal::JSONInputArchive archive(i);
    Services::engine()->root = Node();
    archive(Services::engine()->root);
}