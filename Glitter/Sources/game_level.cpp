#include <fstream>
#include <iostream>
#include <sstream>

#include "game_level.hpp"
#include "resource_manager.hpp"

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
  // Clear any existing bricks
  this->Bricks.clear();
  // Load level data from file and initialize the level
  unsigned int tileCode;
  GameLevel level;
  std::string line;
  std::ifstream fstream(file);
  std::vector<std::vector<unsigned int>> tileData;

  if (fstream)
  {
    // Read the file line by line
    while (std::getline(fstream, line))
    {
      std::istringstream sstream(line);
      std::vector<unsigned int> row;
      // Read each tile code in the line
      while (sstream >> tileCode)
      {
        row.push_back(tileCode);
      }
      tileData.push_back(row);
    }

    // initialize the level with the tile data
    if (!tileData.empty())
    {
      this->init(tileData, levelWidth, levelHeight);
    }
  }
  else
  {
    std::cerr << "ERROR::GAME_LEVEL::LOAD::Failed to read level file: " << file << std::endl;
    return;
  }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
  // iterate through all bricks and draw them
  for (GameObject &brick : this->Bricks)
  {
    if (!brick.Destroyed) // only draw non-destroyed bricks
      brick.Draw(renderer);
  }
}

bool GameLevel::IsCompleted()
{
  // check if all non-solid bricks are destroyed
  for (GameObject &brick : this->Bricks)
  {
    if (!brick.IsSolid && !brick.Destroyed)
      return false; // found a non-solid brick that is not destroyed
  }
  return true; // all non-solid bricks are destroyed
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData,
                     unsigned int levelWidth, unsigned int levelHeight)
{
  // calculate dimensions
  unsigned int height = tileData.size();
  unsigned int width = tileData[0].size();
  float unit_width = levelWidth / static_cast<float>(width);
  float unit_height = levelHeight / static_cast<float>(height);

  // initialize level tiles based on tile data
  for (unsigned int y = 0; y < height; ++y)
  {
    for (unsigned int x = 0; x < width; ++x)
    {
      if (tileData[y][x] > 0) // is a valid tile
      {
        glm::vec2 pos(x * unit_width, y * unit_height);
        glm::vec2 size(unit_width, unit_height);

        if (tileData[y][x] == 1) // solid tile
        {
          glm::vec3 color(0.8f, 0.8f, 0.7f); // default color
          Texture2D texture = ResourceManager::GetTexture("block_solid");
          GameObject brick(pos, size, texture, color);
          brick.IsSolid = true;
          this->Bricks.push_back(brick);
        }
        else if (tileData[y][x] > 1) // non-solid tile
        {
          glm::vec3 color(1.0f);   // default white color
          if (tileData[y][x] == 2) // green tile
            color = glm::vec3(0.2f, 0.8f, 0.2f);
          else if (tileData[y][x] == 3) // blue tile
            color = glm::vec3(0.2f, 0.2f, 0.8f);
          else if (tileData[y][x] == 4) // yellow tile
            color = glm::vec3(0.8f, 0.8f, 0.2f);
          else if (tileData[y][x] == 5) // red tile
            color = glm::vec3(0.8f, 0.2f, 0.2f);

          Texture2D texture = ResourceManager::GetTexture("block");
          GameObject brick(pos, size, texture, color);
          this->Bricks.push_back(brick);
        }
      }
    }
  }
}