#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../services/engine.hpp"
#include "../core/node/components.hpp"

enum class Whence {
    BEGIN,
    CURRENT,
    END
};

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
    File();
    ~File();
    std::string mode;
    std::string path;

    void flush();
    void close();
    void seek(int offset, Whence whence);

    int getSize() const;
    std::string getAsText();
    std::string getAbsolutePath();
    int getPosition();
    std::string getLine();
    bool eofReached() const;
    void storeString(std::string str);
    void storeLine(std::string str);

    static std::string getAbsoluteFilePath(std::string path);
    static File *open(std::string path, std::string mode);
    static int getFileSize(std::string path);
    static bool fileExists(std::string path);
    static std::string getFileAsText(std::string path);

private:
    std::string fsPath;
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
std::string parseFilePath(std::string path);