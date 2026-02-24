#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include "definitions.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "components.hpp"

/**
 * # File handling functions
 * Literally just ``exportNodeTree()`` and ``loadNodeTree()`` for now.
 */

// std::string readFileText(std::string path);
std::string exportNodeTree(std::shared_ptr<Node> node);
std::string exportNodeTree();
void loadNodeTree(std::shared_ptr<Node> parent, std::string jsonText);
void loadNodeTree(std::string jsonText);