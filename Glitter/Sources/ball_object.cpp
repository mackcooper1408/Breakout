#include "ball_object.hpp"

BallObject::BallObject()
    : GameObject(), Radius(12.5f), Stuck(true) {}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity,
                       Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f), sprite, glm::vec3(1.0f), velocity),
      Radius(radius), Stuck(true), Sticky(), PassThrough() {}

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
  // if not stuck, move the ball
  if (!this->Stuck)
  {
    // move the ball
    this->Position += this->Velocity * dt;

    // keep the ball inside the window bounds
    if (this->Position.x <= 0.0f)
    {
      this->Position.x = 0.0f;
      this->Velocity.x = -this->Velocity.x;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
      this->Position.x = window_width - this->Size.x;
      this->Velocity.x = -this->Velocity.x;
    }
    if (this->Position.y <= 0.0f)
    {
      this->Position.y = 0.0f;
      this->Velocity.y = -this->Velocity.y;
    }
  }
  return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
  this->Position = position;
  this->Velocity = velocity;
  this->Stuck = true;
}