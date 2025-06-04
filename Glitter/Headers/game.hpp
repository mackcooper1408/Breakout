#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum GameState
{
  GAME_ACTIVE,
  GAME_MENU,
  GAME_WIN
};

class Game
{
public:
  // Game state
  GameState State;
  bool Keys[1024];
  unsigned int Width, Height;

  // Constructor/Destructor
  Game(unsigned int width, unsigned int height);
  ~Game();

  // Initializes game state (load all shaders/textures/levels)
  void Init();

  // Game loop
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();
};

#endif // GAME_H
