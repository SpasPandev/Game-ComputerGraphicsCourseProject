#include "GameLevel.h"
#include "Application.h"

#include <fstream>
#include <sstream>

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();

    // load from file
    unsigned int blockCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> blockData;

    if (fstream)
    {
        // read each line from level file
        while (std::getline(fstream, line)) 
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;

            // read each word separated by spaces
            while (sstream >> blockCode) 
            {
                row.push_back(blockCode);
            }

            blockData.push_back(row);
        }
        if (blockData.size() > 0)
        {
            this->init(blockData, levelWidth, levelHeight);
        }
    }
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& block : this->Bricks)
    {
        if (!block.Destroyed)
        {
            block.Draw(renderer);
        }
    }
}

void GameLevel::init(std::vector<std::vector<unsigned int>> blockData, unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimensions
    unsigned int height = blockData.size();
    unsigned int width = blockData[0].size();
    float unit_width = levelWidth / static_cast<float>(width);
    float unit_height = levelHeight / height;

    // initialize level blocks based on blockData		
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // check block type from level data (2D level array)
            if (blockData[y][x] == 1) // solid
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, Texture::GetTexture("solid_block"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            }
            else if (blockData[y][x] > 1)	// non-solid
            {
                glm::vec3 color = glm::vec3(1.0f);
                if (blockData[y][x] == 2)
                {
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                }
                else if (blockData[y][x] == 3)
                {
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                }
                else if (blockData[y][x] == 4)
                {
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                }
                else if (blockData[y][x] == 5)
                {
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                }

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, Texture::GetTexture("block"), color));
            }
        }
    }
}