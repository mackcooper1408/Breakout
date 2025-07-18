#ifndef POWER_UP_HPP
#define POWER_UP_HPP

#include "game_object.hpp"

const glm::vec2 SIZE(60.0f, 20.0f);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
  std::string Type;
  float Duration;
  bool Activated;

  PowerUp(std::string type, glm::vec3 color, float duration,
          glm::vec2 position, Texture2D texture)
      : GameObject(position, SIZE, texture, color, VELOCITY),
        Type(type), Duration(duration), Activated()
  {
  }
};

#endif
