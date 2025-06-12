#include "game.hpp"
#include "resource_manager.hpp"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

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

  // Load textures
  ResourceManager::LoadTexture("Glitter/Textures/background.jpg", false, "background");
  ResourceManager::LoadTexture("Glitter/Textures/awesomeface.png", true, "face");
  ResourceManager::LoadTexture("Glitter/Textures/block.png", false, "block");
  ResourceManager::LoadTexture("Glitter/Textures/block_solid.png", false, "block_solid");
  ResourceManager::LoadTexture("Glitter/Textures/paddle.png", true, "paddle");

  // Load levels
  GameLevel one, two, three, four;
  one.Load("Glitter/Levels/one.lvl", this->Width, this->Height / 2);
  two.Load("Glitter/Levels/two.lvl", this->Width, this->Height / 2);
  three.Load("Glitter/Levels/three.lvl", this->Width, this->Height / 2);
  four.Load("Glitter/Levels/four.lvl", this->Width, this->Height / 2);
  this->Levels.push_back(one);
  this->Levels.push_back(two);
  this->Levels.push_back(three);
  this->Levels.push_back(four);
  this->CurrentLevel = 0;

  // Initialize player paddle
  glm::vec2 playerPos = glm::vec2(
      this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y - 20.0f);
  this->Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

  // Set initial game state
  // TODO: SET VIA MENU
  this->State = GAME_ACTIVE;
}

void Game::ProcessInput(float dt)
{
  // Handle user input, update game state based on input
  // This function should check for key presses and update the game state accordingly.
  if (this->State == GAME_ACTIVE)
  {
    float velocity = PLAYER_VELOCITY * dt; // Calculate velocity based on delta time

    if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])
    {
      // Move player paddle left
      if (this->Player->Position.x >= 0.0f)
        this->Player->Position.x -= velocity;
    }
    if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
    {
      // Move player paddle right
      if (this->Player->Position.x <= this->Width - this->Player->Size.x)
        this->Player->Position.x += velocity;
    }
  }
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
  if (this->State == GAME_ACTIVE)
  {
    // Draw background
    this->Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
                               glm::vec2(static_cast<float>(this->Width), static_cast<float>(this->Height)));
    // Draw current level
    this->Levels[this->CurrentLevel].Draw(*this->Renderer);
    // Draw player paddle
    this->Player->Draw(*this->Renderer);
  }
}
