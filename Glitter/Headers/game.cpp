#include "game.hpp"

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Width(width), Height(height)
{
}

Game::~Game()
{
}

void Game::Init()
{
  // Initialize game state, load shaders, textures, etc.
  // This is where you would set up your game resources.
}

void Game::ProcessInput(float dt)
{
  // Handle user input, update game state based on input
  // This function should check for key presses and update the game state accordingly.
}

void Game::Update(float dt)
{
  // Update game logic, physics, etc.
  // This function should update the game state based on the elapsed time since the last frame.
}

void Game::Render()
{
  // Render the game scene
  // This function should draw the game objects to the screen.
}
