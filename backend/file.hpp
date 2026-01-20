#pragma once

#include <SDL.h>
#include "definitions.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "components.hpp"

std::string readFileText(std::string path);
std::string exportNodeTree(Engine *engine);
void loadNodeTree(Engine *engine, std::string jsonText);