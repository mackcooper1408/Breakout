#include <tuple>

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
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width),
                                    static_cast<float>(Height), 0.0f, -1.0f, 1.0f);

  ResourceManager::GetShader("sprite").Use();
  ResourceManager::GetShader("sprite").SetInteger("spriteTexture", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
  Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

  // Load textures
  ResourceManager::LoadTexture("Glitter/Textures/background.jpg", false, "background");
  ResourceManager::LoadTexture("Glitter/Textures/awesomeface.png", true, "face");
  ResourceManager::LoadTexture("Glitter/Textures/block.png", false, "block");
  ResourceManager::LoadTexture("Glitter/Textures/block_solid.png", false, "block_solid");
  ResourceManager::LoadTexture("Glitter/Textures/paddle.png", true, "paddle");

  // Load levels
  GameLevel one, two, three, four;
  one.Load("Glitter/Levels/one.lvl", Width, Height / 2);
  two.Load("Glitter/Levels/two.lvl", Width, Height / 2);
  three.Load("Glitter/Levels/three.lvl", Width, Height / 2);
  four.Load("Glitter/Levels/four.lvl", Width, Height / 2);
  Levels.push_back(one);
  Levels.push_back(two);
  Levels.push_back(three);
  Levels.push_back(four);
  CurrentLevel = 0;

  // Initialize player paddle
  glm::vec2 playerPos = glm::vec2(
      Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
  Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

  // Initialize ball object
  glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                            -BALL_RADIUS * 2.0f);
  Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                        ResourceManager::GetTexture("face"));

  // Set initial game state
  // TODO: SET VIA MENU
  State = GAME_ACTIVE;
}

void Game::ProcessInput(float dt)
{
  // Handle user input, update game state based on input
  // This function should check for key presses and update the game state accordingly.
  if (State == GAME_ACTIVE)
  {
    float velocity = PLAYER_VELOCITY * dt; // Calculate velocity based on delta time

    if (Keys[GLFW_KEY_A] || Keys[GLFW_KEY_LEFT])
    {
      // Move player paddle left
      if (Player->Position.x >= 0.0f)
      {
        Player->Position.x -= velocity;
        if (Ball->Stuck)
        {
          // If the ball is stuck, move it with the paddle
          Ball->Position.x -= velocity;
        }
      }
    }
    if (Keys[GLFW_KEY_D] || Keys[GLFW_KEY_RIGHT])
    {
      // Move player paddle right
      if (Player->Position.x <= Width - Player->Size.x)
      {
        Player->Position.x += velocity;
        if (Ball->Stuck)
        {
          // If the ball is stuck, move it with the paddle
          Ball->Position.x += velocity;
        }
      }
    }
    if (Keys[GLFW_KEY_SPACE])
    {
      // Release the ball from the paddle
      Ball->Stuck = false;
    }
  }
}

void Game::Update(float dt)
{
  // Update game logic, physics, etc.
  // This function should update the game state based on the elapsed time since the last frame.
  Ball->Move(dt, Width);
  DoCollisions();

  if (Ball->Position.y >= Height)
  {
    ResetLevel();
    ResetPlayer();
  }
}

void Game::Render()
{
  // Render the game scene
  // This function should draw the game objects to the screen.;
  if (State == GAME_ACTIVE)
  {
    // Draw background
    Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
                         glm::vec2(static_cast<float>(Width), static_cast<float>(Height)));
    // Draw current level
    Levels[CurrentLevel].Draw(*Renderer);
    // Draw player paddle
    Player->Draw(*Renderer);
    // Draw ball object
    Ball->Draw(*Renderer);
  }
}

void Game::DoCollisions()
{
  // Check for collisions between the ball and the player paddle
  Collision collision = CheckCollision(*Ball, *Player);
  if (!Ball->Stuck && std::get<0>(collision))
  {
    float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
    float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    float percentage = distance / (Player->Size.x / 2.0f);

    float strength = 2.0f;
    glm::vec2 oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    Ball->Velocity.y = -1.0f * std::abs(oldVelocity.y);
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
  }

  // Check for collisions with blocks in the current level
  GameLevel &currentLevel = Levels[CurrentLevel];
  for (GameObject &block : currentLevel.Bricks)
  {
    if (block.Destroyed)
      continue; // Skip destroyed blocks

    Collision blockCollision = CheckCollision(*Ball, block);
    if (std::get<0>(blockCollision))
    {
      if (!block.IsSolid)
        block.Destroyed = true; // Mark block as destroyed

      Direction dir = std::get<1>(blockCollision);
      glm::vec2 diff = std::get<2>(blockCollision);

      if (dir == LEFT || dir == RIGHT)
      {
        Ball->Velocity.x = -Ball->Velocity.x;
        float penetration = Ball->Radius - std::abs(diff.x);
        if (dir == LEFT)
          Ball->Position.x += penetration;
        else
          Ball->Position.x -= penetration;
      }
      else
      {
        Ball->Velocity.y = -Ball->Velocity.y;
        float penetration = Ball->Radius - std::abs(diff.y);
        if (dir == UP)
          Ball->Position.y -= penetration;
        else
          Ball->Position.y += penetration;
      }
    }
  }
}

/*
 * Private helper functions
 */

void Game::ResetLevel()
{
  for (GameObject &brick : Levels[CurrentLevel].Bricks)
  {
    brick.Destroyed = false;
  }
}

void Game::ResetPlayer()
{
  Player->Position.x = Width / 2.0f - (PLAYER_SIZE.x / 2.0f);
  Ball->Stuck = true;
  Ball->Position = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                                -BALL_RADIUS * 2.0f);
}

Direction Game::VectorDirection(glm::vec2 target)
{
  // Determine the direction of a vector
  // This function should return the direction of the vector as an enum value.
  glm::vec2 compass[] = {
      glm::vec2(0.0f, 1.0f),  // UP
      glm::vec2(1.0f, 0.0f),  // RIGHT
      glm::vec2(0.0f, -1.0f), // DOWN
      glm::vec2(-1.0f, 0.0f)  // LEFT
  };

  glm::vec2 normalized = glm::normalize(target);
  float maxCosine = 0.0f;
  unsigned int bestMatch = -1;
  for (unsigned int i = 0; i < 4; ++i)
  {
    float cosine = glm::dot(normalized, compass[i]);
    if (cosine > maxCosine)
    {
      maxCosine = cosine;
      bestMatch = i;
    }
  }
  return static_cast<Direction>(bestMatch);
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

Collision Game::CheckCollision(BallObject &one, GameObject &two)
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

  bool collided = glm::length(difference) <= one.Radius;
  if (!collided)
    return std::make_tuple(collided, UP, glm::vec2(0.0f, 0.0f));

  return std::make_tuple(collided, VectorDirection(difference), difference);
}
