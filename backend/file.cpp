#include "file.hpp"
#include <iostream>
#include <iomanip>
#include "logger.hpp"
#include "services.hpp"

std::string exportNodeTree(std::shared_ptr<Node> node)
{
    std::stringstream o;
    {
        cereal::JSONOutputArchive archive(o);
        archive(node);
    }
    return o.str();
}

/**
 * This function is specifically for exporting the root node of the engine.
 */
std::string exportNodeTree()
{
    std::stringstream o;
    {
        cereal::JSONOutputArchive archive(o);
        archive(Services::engine()->root);
    }
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