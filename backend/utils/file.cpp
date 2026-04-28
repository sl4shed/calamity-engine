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

void File::fileCopy(const std::string& path1, const std::string& path2)
{
    if (!SDL_CopyFile(parseFilePath(path1).c_str(), parseFilePath(path2).c_str()))
    {
        Logger::error("Failed to copy file {} to {}: {}", path1, path2, SDL_GetError());
    }
}

void File::fileDelete(const std::string& path)
{
    if (!SDL_RemovePath(parseFilePath(path).c_str()))
    {
        Logger::error("Failed to delete file {}: {}", path, SDL_GetError());
    }
}

void File::directoryCreate(const std::string& path)
{
    if (!SDL_CreateDirectory(parseFilePath(path).c_str()))
    {
        Logger::error("Failed to create directory {}: {}", path, SDL_GetError());
    }
}

std::vector<std::string> File::directoryEnumerate(const std::string& path)
{
    std::vector<std::string> entries;

    if (!SDL_EnumerateDirectory(parseFilePath(path).c_str(),
        [](void* userdata, const char* dirname, const char* name)
        {
            auto* vec = static_cast<std::vector<std::string>*>(userdata);
            vec->emplace_back(name);
            return SDL_ENUM_CONTINUE;
        },
        &entries))
    {
        Logger::error("Failed to enumerate directory {}: {}", path, SDL_GetError());
    }

    return entries;
}

File *File::open(std::string path, const std::string& mode) {
    const auto file = new File();
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

void File::flush() const
{
    if(handle) {
        if (!SDL_FlushIO(handle))
        {
            Logger::error("Failed to flush file {}: {}", path, SDL_GetError());
        }
    }
}

void File::close() {
    if(handle) {
        if (!SDL_CloseIO(handle))
        {
            Logger::error("Failed to close file {}: {}", path, SDL_GetError());
        }
        handle = nullptr;
    }
}

void File::seek(const int offset, Whence whence) const
{
    if(handle) {
        if (SDL_SeekIO(handle, offset, static_cast<SDL_IOWhence>(whence)) == -1)
        {
            Logger::error("Failed to seek file {}: {}", path, SDL_GetError());
        }
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
        const int size = pathInfo.size;
        return size;
    }
    Logger::warn("Error encountered while getting file size for {}: {}", path, SDL_GetError());

    return -1;
}

std::string File::getAsText() {
    if(handle) {
        const int size = getSize();
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

void File::storeString(const std::string& str) {
    if (handle) {
        if (!SDL_WriteIO(handle, str.c_str(), str.size()))
        {
            Logger::error("Failed to store string '{}' in file {}: {}", str, path, SDL_GetError());
        }
        return;
    }
    Logger::warn("Attempted to write string to file with no handle: {}", path);
}

void File::storeLine(const std::string& str) {
    if(!str.empty() && str.back() == '\n') {
        storeString(str);
        return;
    }

    storeString(str + "\n");
}

std::string File::getAbsoluteFilePath(const std::string& path) {
    return parseFilePath(path);
}

int File::getFileSize(const std::string& path) {
    const std::string fsPath = parseFilePath(path);
    SDL_PathInfo pathInfo;
    if(SDL_GetPathInfo(fsPath.c_str(), &pathInfo) == true) {
        const int size = pathInfo.size;
        return size;
    }

    Logger::error("Failed to get file size of file {}: {}", path, SDL_GetError());
    return -1;
}

bool File::fileExists(const std::string& path) {
    return SDL_GetPathInfo(parseFilePath(path).c_str(), nullptr);
}

std::string File::getFileAsText(std::string path) {
    const std::string fsPath = parseFilePath(path);
    if(SDL_IOStream *stream = SDL_IOFromFile(fsPath.c_str(), "r")) {
        const int size = getFileSize(path); // im too lazy and it's basically the same shit /shrug
        if(size < 0) {
            Logger::error("Failed to get file {} as text: size is below 0", path);
            return "";
        };

        std::string text(size, '\0');
        if (!SDL_ReadIO(stream, &text[0], size))
        {
            Logger::error("Failed to read file {} as text: {}", path, SDL_GetError());
        }

        if (!SDL_CloseIO(stream))
        {
            Logger::error("Failed to close file {} while trying to get as text: {}", path, SDL_GetError());
        }
        return text;
    }

    Logger::error("Failed to get file {} as text: {}", path, SDL_GetError());
    return "";
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
void loadNodeTree(const std::shared_ptr<Node>& parent, const std::string& jsonText)
{
    std::stringstream i(jsonText);
    cereal::JSONInputArchive archive(i);
    auto node = std::make_shared<Node>();
    archive(node);
    parent->addChild(node);
}

/**
 * This function is specifically for loading the root node of the engine.
 */
void loadNodeTree(const std::string& jsonText)
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
    }

    if(path.rfind("res://", 0) == 0) {
        std::string basePath = SDL_GetBasePath();
        return std::string(basePath) + path.substr(6);
    }

    Logger::warn("File path {} does not start with a valid prefix (user:// or res://)", path);
    return "";
}