#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tuple>

#include "sprite_renderer.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "ball_object.hpp"
#include "particle_generator.hpp"
#include "post_processor.hpp"
#include "power_up.hpp"

enum GameState
{
  GAME_ACTIVE,
  GAME_MENU,
  GAME_WIN
};

enum Direction
{
  UP,
  RIGHT,
  DOWN,
  LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game
{
public:
  // Game state
  GameState State;
  bool Keys[1024];
  unsigned int Width, Height;
  std::vector<GameLevel> Levels;
  unsigned int CurrentLevel;

  std::vector<PowerUp> PowerUps;

  // Constructor/Destructor
  Game(unsigned int width, unsigned int height);
  ~Game();

  // Initializes game state (load all shaders/textures/levels)
  void Init();

  // Game loop
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();

  void DoCollisions();

  void SpawnPowerUps(GameObject &block);
  void UpdatePowerUps(float dt);

private:
  SpriteRenderer *Renderer;
  GameObject *Player;
  BallObject *Ball;
  ParticleGenerator *Particles;
  PostProcessor *Effects;

  // Reset Helpers
  void ResetLevel();
  void ResetPlayer();

  // collision helpers
  bool CheckCollision(GameObject &one, GameObject &two);
  Collision CheckCollision(BallObject &one, GameObject &two);
  Direction VectorDirection(glm::vec2 target);

  // Powerup Helpers
  bool ShouldSpawn(unsigned int chance);
  void ActivatePowerUp(PowerUp &powerUp);
  bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);
};

#endif // GAME_H
