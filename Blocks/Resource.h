#pragma once

#include <map>
#include "Shader.h"
#include "Texture.h"

// resource storage
static std::map<std::string, Shader>    Shaders;
static std::map<std::string, Texture> Textures;