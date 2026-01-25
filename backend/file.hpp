#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "definitions.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "components.hpp"

std::string readFileText(std::string path);
std::string exportNodeTree(Node *node);
void loadNodeTree(Node *parent, std::string jsonText);