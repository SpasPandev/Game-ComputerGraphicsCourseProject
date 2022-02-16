#pragma once

#include "GameObject.h"
#include "SpriteRenderer.h"

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class GameLevel
{
public:
    std::vector<GameObject> Bricks;

    GameLevel() { }

    // loads level from file
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

    // render level
    void Draw(SpriteRenderer& renderer);

private:
    // initialize level from block data
    void init(std::vector<std::vector<unsigned int>> blockData, unsigned int levelWidth, unsigned int levelHeight);
};