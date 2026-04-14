#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../services/engine.hpp"
#include "../core/node/components.hpp"

/**
 * # File
 *
 * user://path/to/file
 * res://path/to/file
 *
 */
class File
{
public:
    File(std::string path, std::string mode);
    ~File();
    const std::string mode;
    const std::string path;

    int getSize(std::string path);
    void flush();
    void close();

    std::string getAsText();
    std::string getPath();
    std::string getAbsolutePath(std::string path);

    static bool fileExists(std::string path);
    static std::string getFileAsText(std::string path);
private:
    SDL_IOStream *handle;
};

/**
 * # File handling functions
 * Literally just ``exportNodeTree()`` and ``loadNodeTree()`` for now.
 */

// std::string readFileText(std::string path);
std::string exportNodeTree(std::shared_ptr<Node> node);
std::string exportNodeTree();
void loadNodeTree(std::shared_ptr<Node> parent, std::string jsonText);
void loadNodeTree(std::string jsonText);