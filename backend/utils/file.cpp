#include "file.hpp"
#include <iostream>
#include <iomanip>
#include "logger.hpp"
#include "../services/services.hpp"

File::~File()
{
    if(handle) {
        SDL_CloseIO(handle);
        handle = nullptr;
    }
}


File *File::open(std::string path, std::string mode) {
    File *file = new File();
    file->path = path;
    file->mode = mode;
    file->fsPath = parseFilePath(path);
    file->handle = SDL_IOFromFile(file->fsPath.c_str(), mode.c_str());
    if(!file->handle) {
        Logger::error("Failed to open file: {}", path);
        delete file;
        return nullptr;
    }

    return file;
}

void File::flush() {
    if(handle) {
        SDL_FlushIO(handle);
    }
}

void File::close() {
    if(handle) {
        SDL_CloseIO(handle);
        handle = nullptr;
    }
}

void File::seek(int offset, Whence whence) {
    if(handle) {
        SDL_SeekIO(handle, offset, (SDL_IOWhence)whence);
    }
}

int File::getPosition() {
    if(handle) {
        return SDL_TellIO(handle);
    }

    Logger::warn("Attempted to get position of file with no handle: {}", path);
    return -1;
}

int File::getSize() const {
    SDL_PathInfo pathInfo;
    if(SDL_GetPathInfo(fsPath.c_str(), &pathInfo) == true) {
        int size = pathInfo.size;
        return size;
    } else {
        Logger::warn("Error encountered while getting file size for {}: {}", path, SDL_GetError());
    }

    return -1;
}

std::string File::getAsText() {
    if(handle) {
        int size = getSize();
        if(size < 0) return "";

        std::string text(size, '\0');
        SDL_ReadIO(handle, &text[0], size);
        return text;
    }

    Logger::warn("Attempted to read file as text with no handle: {}", path);
    return "";
}

std::string File::getAbsolutePath() {
    return fsPath;
}

std::string File::getLine() {
    if(handle) {
        std::string line;
        char c;
        while(SDL_ReadIO(handle, &c, 1) == 1) {
            if(c == '\n') break;
            line += c;
        }
        return line;
    }

    Logger::warn("Attempted to read line from file with no handle: {}", path);
    return "";
}

bool File::eofReached() const {
    if (handle)
    {
        return SDL_GetIOStatus(handle) == SDL_IO_STATUS_EOF;
    }
    
    Logger::warn("Attempted to check EOF on file with no handle: {}", path);
    return true;
}

void File::storeString(std::string str) {
    if (handle) {
        SDL_WriteIO(handle, str.c_str(), str.size());
        return;
    } else {
        Logger::warn("Attempted to write string to file with no handle: {}", path);
    }
}

void File::storeLine(std::string str) {
    if(!str.empty() && str.back() == '\n') {
        storeString(str);
    } else {
        storeString(str + "\n");
    }
}

std::string File::getAbsoluteFilePath(std::string path) {
    return parseFilePath(path);
}

int File::getFileSize(std::string path) {
    std::string fsPath = parseFilePath(path);
    SDL_PathInfo pathInfo;
    if(SDL_GetPathInfo(fsPath.c_str(), &pathInfo) == true) {
        int size = pathInfo.size;
        return size;
    } else {
        return -1;
    }
}

bool File::fileExists(std::string path) {
    return SDL_GetPathInfo(parseFilePath(path).c_str(), nullptr);
}

std::string File::getFileAsText(std::string path) {
    std::string fsPath = parseFilePath(path);
    SDL_IOStream *stream = SDL_IOFromFile(fsPath.c_str(), "r");
    if(stream) {
        int size = File::getFileSize(path); // im too lazy and it's basically the same shit /shrug
        if(size < 0) return "";

        std::string text(size, '\0');
        SDL_ReadIO(stream, &text[0], size);
        SDL_CloseIO(stream);
        return text;
    }
}

/**
 * This function is for exporting any node as a JSON file.
 */
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

/**
 * This function is for loading a node tree from JSON text.
 */
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

/**
 * This function takes a path like "user://myfile.txt" or "res://myfile.txt" and returns the actual file path on the filesystem.
 */
std::string parseFilePath(std::string path) {
    if(path.rfind("user://", 0) == 0) {
        std::string userPath = SDL_GetPrefPath("Calamity Engine", Services::engine()->appName.c_str());
        return std::string(userPath) + path.substr(7);
    } else if(path.rfind("res://", 0) == 0) {
        std::string basePath = SDL_GetBasePath();
        return std::string(basePath) + path.substr(6);
    } else {
        Logger::warn("File path {} does not start with a valid prefix (user:// or res://)", path);
        return "";
    }
}