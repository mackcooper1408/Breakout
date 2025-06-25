#ifndef BALLOBJECT_HPP
#define BALLOBJECT_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.hpp"

class BallObject : public GameObject
{
public:
  // Ball properties
  float Radius;
  bool Stuck;

  // PowerUp Effects
  bool Sticky;
  bool PassThrough;

  // Constructor
  BallObject();
  BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

  // Move the ball, keeping it inside the window bounds
  glm::vec2 Move(float dt, unsigned int window_width);
  void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // BALLOBJECT_HPP