#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Texture.h"

class BallObject : public GameObject
{
public:
    float Radius;
    bool Stuck;

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture sprite);

    // moves the ball
    glm::vec2 Move(float dt, unsigned int window_width);

    // resets ball position
    void Reset(glm::vec2 position, glm::vec2 velocity);
};