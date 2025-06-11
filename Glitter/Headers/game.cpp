#include "game.hpp"
#include "resource_manager.hpp"

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
  ResourceManager::LoadShader("Glitter/Shaders/sprite_shader.vert", "Glitter/Shaders/sprite_shader.frag",
                              nullptr, "sprite");
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

  ResourceManager::GetShader("sprite").Use();
  ResourceManager::GetShader("sprite").SetInteger("spriteTexture", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
  this->Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
  ResourceManager::LoadTexture("Glitter/Textures/awesomeface.png", true, "face");
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
  // This function should draw the game objects to the screen.;
  this->Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f),
                             glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
