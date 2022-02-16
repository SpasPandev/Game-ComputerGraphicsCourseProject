#include "Application.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "BallObject.h"
#include "GameLevel.h"
#include "Resource.h"



// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;

Application::Application(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Application::~Application()
{
    delete Renderer;
    delete Player;
    delete Ball;
}

void Application::Init()
{
    // load shaders
    Shader::LoadShader("res/vertex.glsl", "res/fragment.glsl", "shader");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    Shader::GetShader("shader").Use().SetInteger("image", 0);
    Shader::GetShader("shader").SetMatrix4("projection", projection);

    // set render-specific controls
    Renderer = new SpriteRenderer(Shader::GetShader("shader"));

    // load textures
    Texture::LoadTexture("textures/background.jpg", false, "background");
    Texture::LoadTexture("textures/ball.png", true, "ball");
    Texture::LoadTexture("textures/block.png", false, "block");
    Texture::LoadTexture("textures/solid_block.png", false, "solid_block");
    Texture::LoadTexture("textures/paddle.png", true, "paddle");

    // load level
    GameLevel one; 
    one.Load("level/first.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Level = 0;

    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - Paddle_Size.x / 2.0f, this->Height - Paddle_Size.y);
    Player = new GameObject(playerPos, Paddle_Size, Texture::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(Paddle_Size.x / 2.0f - Ball_Radius, -Ball_Radius * 2.0f);
    Ball = new BallObject(ballPos, Ball_Radius, Ball_Velocity, Texture::GetTexture("ball"));
}

void Application::Update(float dt)
{
    // update objects
    Ball->Move(dt, this->Width);

    // check for collisions
    this->DoCollisions();

    // if ball reach bottom edge
    if (Ball->Position.y >= this->Height)
    {
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Application::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = Paddle_Velocity * dt;

        // move paddle
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                {
                    Ball->Position.x -= velocity;
                }
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                {
                    Ball->Position.x += velocity;
                }
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
        {
            Ball->Stuck = false;
        }
    }
}

void Application::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(Texture::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

        // draw level
        this->Levels[this->Level].Draw(*Renderer);

        // draw player
        Player->Draw(*Renderer);

        // draw ball
        Ball->Draw(*Renderer);
    }
}


void Application::ResetLevel()
{
    this->Levels[0].Load("level/first.lvl", this->Width, this->Height / 2);
}

void Application::ResetPlayer()
{
    // reset paddle/ball
    Player->Position = glm::vec2(this->Width / 2.0f - Paddle_Size.x / 2.0f, this->Height - Paddle_Size.y);
    Ball->Reset(Player->Position + glm::vec2(Paddle_Size.x / 2.0f - Ball_Radius, -(Ball_Radius * 2.0f)), Ball_Velocity);
}

// collision detection

//bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

void Application::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);

            // if collision is true
            if (std::get<0>(collision)) 
            {
                // destroy block if not solid
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                }

                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);

                // horizontal collision
                if (dir == LEFT || dir == RIGHT) 
                {
                    // reverse horizontal velocity
                    Ball->Velocity.x = -Ball->Velocity.x; 

                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);

                    if (dir == LEFT)
                    {
                        Ball->Position.x += penetration;
                    }
                    else
                    {
                        Ball->Position.x -= penetration;
                    }
                }
                // vertical collision
                else 
                {
                    // reverse vertical velocity
                    Ball->Velocity.y = -Ball->Velocity.y; 

                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                    {
                        Ball->Position.y -= penetration;
                    }
                    else
                    {
                        Ball->Position.y += penetration;
                    }
                }
            }
        }
    }

    // check collisions for paddle and ball (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);

    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);

        // move
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = Ball_Velocity.x * percentage * strength;

        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);

        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}

//bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
//{
//    // collision x-axis?
//    bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
//
//    // collision y-axis?
//    bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
//
//    // collision only if on both axes
//    return collisionX && collisionY;
//}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // circle center
    glm::vec2 center(one.Position + one.Radius);

    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    // value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;

    // vector between center circle and closest point AABB 
    difference = closest - center;

    // check if length < radius
    if (glm::length(difference) < one.Radius) 
    {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    else
    {
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    }
}

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };

    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Application::Clear()
{
    //delete all shaders	
    for (auto iter : Shaders)
    {
        glDeleteProgram(iter.second.ID);
    }

    // delete all textures
    for (auto iter : Textures)
    {
        glDeleteTextures(1, &iter.second.ID);
    }
}



