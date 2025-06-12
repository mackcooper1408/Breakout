#include "game.hpp"
#include "resource_manager.hpp"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

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

  // Initialize ball object
  glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                            -BALL_RADIUS * 2.0f);
  this->Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                              ResourceManager::GetTexture("face"));

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
      {
        this->Player->Position.x -= velocity;
        if (this->Ball->Stuck)
        {
          // If the ball is stuck, move it with the paddle
          this->Ball->Position.x -= velocity;
        }
      }
    }
    if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
    {
      // Move player paddle right
      if (this->Player->Position.x <= this->Width - this->Player->Size.x)
      {
        this->Player->Position.x += velocity;
        if (this->Ball->Stuck)
        {
          // If the ball is stuck, move it with the paddle
          this->Ball->Position.x += velocity;
        }
      }
    }
    if (this->Keys[GLFW_KEY_SPACE])
    {
      // Release the ball from the paddle
      this->Ball->Stuck = false;
    }
  }
}

void Game::Update(float dt)
{
  // Update game logic, physics, etc.
  // This function should update the game state based on the elapsed time since the last frame.
  this->Ball->Move(dt, this->Width);
  this->DoCollisions();
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
    // Draw ball object
    this->Ball->Draw(*this->Renderer);
  }
}

bool Game::CheckCollision(GameObject &one, GameObject &two)
{
  // Check for collision between two game objects
  // This function should return true if the two game objects are colliding.
  bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                    two.Position.x + two.Size.x >= one.Position.x;
  bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                    two.Position.y + two.Size.y >= one.Position.y;
  return collisionX && collisionY;
}

bool Game::CheckCollision(BallObject &one, GameObject &two)
{
  // Check for collision between a ball object and a game object
  // This function should return true if the two game objects are colliding.
  glm::vec2 ballCenter(one.Position + one.Radius);
  glm::vec2 aabbHalfSize(two.Size / 2.0f);
  glm::vec2 aabbCenter(two.Position + aabbHalfSize);

  glm::vec2 difference = ballCenter - aabbCenter;
  glm::vec2 clamped = glm::clamp(difference, -aabbHalfSize, aabbHalfSize);

  // Find the closest point on the AABB to the ball center
  glm::vec2 closest = aabbCenter + clamped;
  // Calculate the distance between the closest point and the ball center
  difference = closest - ballCenter;

  return glm::length(difference) < one.Radius;
}

void Game::DoCollisions()
{
  // Check for collisions between the ball and the player paddle
  if (this->CheckCollision(*this->Ball, *this->Player))
  {
    this->Ball->Velocity.y = -this->Ball->Velocity.y; // Reverse ball's vertical velocity
  }

  // Check for collisions with blocks in the current level
  GameLevel &currentLevel = this->Levels[this->CurrentLevel];
  for (GameObject &block : currentLevel.Bricks)
  {
    if (block.Destroyed)
      continue; // Skip destroyed blocks
    if (this->CheckCollision(*this->Ball, block))
    {
      if (block.IsSolid)
      {
        // Handle collision with solid blocks
        this->Ball->Velocity.y = -this->Ball->Velocity.y; // Reverse ball's vertical velocity
      }
      else if (!block.IsSolid)
      {
        // Handle collision with non-solid blocks
        this->Ball->Velocity.y = -this->Ball->Velocity.y; // Reverse ball's vertical velocity
        block.Destroyed = true;                           // Mark block as destroyed
      }
    }
  }
}
