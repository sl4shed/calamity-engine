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
 * Can be used to access the host filesystem. Generally, you would only want access to an app-specific folder (for save data, for example) or to the root of the app. So, that's exactly what you get!
 *
 * When trying to access a file anywhere in calamity engine, you can use the `user://` and `res://` prefixes to access the user folder and the base folder of the app respectively:
 * ```cpp
 * File *save = File::open("user://savedata.txt", "r");
 * std::string saveText = save->getAsText();
 * Logger::info("save data: {}", saveText);
 *
 * // Output: yeah
 * ```
 *
 * You can also write to files:
 * ```cpp
 * File *save = File::open("user://savedata.txt", "a+");
 * save->seek(0, Whence::END);
 * save->storeString("yeah 2");
 *
 * Logger::info("{}", save->getAsText());
 * // Output: yeah
 * // Output: yeah 2
 * ```
 *
 * If you structured your project like was recommended in the getting started guide, you access your game assets like so:
 * ```
 *
 * ```
 */
class File
{
public:
    ~File();
    std::string mode;
    std::string path;

    void flush() const;
    void close();
    void seek(int offset, Whence whence) const;

    int getSize() const;
    std::string getAsText();
    std::string getAbsolutePath();
    int getPosition();
    std::string getLine();
    bool eofReached() const;
    void storeString(const std::string& str);
    void storeLine(const std::string& str);

    static std::string getAbsoluteFilePath(const std::string& path);
    static File *open(std::string path, const std::string& mode);
    static int getFileSize(const std::string& path);
    static bool fileExists(const std::string& path);
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
void loadNodeTree(const std::shared_ptr<Node>& parent, const std::string& jsonText);
void loadNodeTree(const std::string& jsonText);
std::string parseFilePath(std::string path);