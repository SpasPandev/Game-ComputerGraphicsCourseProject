#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

#include "GameLevel.h"
#include "Shader.h"
#include "Texture.h"

// current state of the game
enum GameState {
    GAME_ACTIVE
};

// four possible (collision) directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision;

const glm::vec2 Paddle_Size(100.0f, 20.0f);

const float Paddle_Velocity(500.0f);

const glm::vec2 Ball_Velocity(100.0f, -350.0f);

const float Ball_Radius = 12.5f;

class Application
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel>  Levels;
    unsigned int            Level;

    Application(unsigned int width, unsigned int height);
    ~Application();

    // (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();

    // reset
    void ResetLevel();
    void ResetPlayer();

    // clear all loaded resources
    static void      Clear();
};