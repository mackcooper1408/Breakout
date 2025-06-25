#include <tuple>
#include <string>

#include "game.hpp"
#include "resource_manager.hpp"

const std::string g_project_source_dir = PROJECT_SOURCE_DIR;
const std::string basePath = g_project_source_dir + "/Glitter";

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Width(width), Height(height)
{
}

Game::~Game()
{
  delete Renderer;
  delete Player;
  delete Ball;
  delete Particles;
  delete Effects;
}

void Game::Init()
{
  // Set game projection matrix
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width),
                                    static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
  // Load shaders
  ResourceManager::LoadShader((basePath + "/Shaders/sprite_shader.vert").c_str(),
                              (basePath + "/Shaders/sprite_shader.frag").c_str(),
                              nullptr, "sprite");
  ResourceManager::LoadShader((basePath + "/Shaders/particle_shader.vert").c_str(),
                              (basePath + "/Shaders/particle_shader.frag").c_str(),
                              nullptr, "particle");
  ResourceManager::LoadShader((basePath + "/Shaders/post_proc_shader.vert").c_str(),
                              (basePath + "/Shaders/post_proc_shader.frag").c_str(),
                              nullptr, "effects");

  ResourceManager::GetShader("sprite").Use();
  ResourceManager::GetShader("sprite").SetInteger("spriteTexture", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

  ResourceManager::GetShader("particle").Use();
  ResourceManager::GetShader("particle").SetInteger("sprite", 0);
  ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

  Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

  Effects = new PostProcessor(ResourceManager::GetShader("effects"), Width * 2, Height * 2);

  // Load textures
  ResourceManager::LoadTexture(
      (basePath + "/Textures/background.jpg").c_str(), false, "background");
  ResourceManager::LoadTexture((basePath + "/Textures/awesomeface.png").c_str(), true, "face");
  ResourceManager::LoadTexture((basePath + "/Textures/block.png").c_str(), false, "block");
  ResourceManager::LoadTexture(
      (basePath + "/Textures/block_solid.png").c_str(), false, "block_solid");
  ResourceManager::LoadTexture((basePath + "/Textures/paddle.png").c_str(), true, "paddle");
  ResourceManager::LoadTexture((basePath + "/Textures/particle.png").c_str(), true, "particle");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_chaos.png").c_str(), true, "tex_chaos");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_confuse.png").c_str(), true, "tex_confuse");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_increase.png").c_str(), true, "tex_increase");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_passthrough.png").c_str(), true, "tex_pass");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_speed.png").c_str(), true, "tex_speed");
  ResourceManager::LoadTexture((basePath + "/Textures/powerup_sticky.png").c_str(), true, "tex_sticky");

  // Load levels
  GameLevel one, two, three, four, solid;
  one.Load((basePath + "/Levels/one.lvl").c_str(), Width, Height / 2);
  two.Load((basePath + "/Levels/two.lvl").c_str(), Width, Height / 2);
  three.Load((basePath + "/Levels/three.lvl").c_str(), Width, Height / 2);
  four.Load((basePath + "/Levels/four.lvl").c_str(), Width, Height / 2);

  // solid.Load((basePath + "/Levels/solid.lvl").c_str(), Width, Height / 2);
  // Levels.push_back(solid);

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

  // Inititalize the Particle Generator
  Particles = new ParticleGenerator(
      ResourceManager::GetShader("particle"),
      ResourceManager::GetTexture("particle"),
      500);
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

  Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

  UpdatePowerUps(dt);

  if (Ball->Position.y >= Height)
  {
    ResetLevel();
    ResetPlayer();
  }

  if (ShakeTime > 0.0f)
  {
    ShakeTime -= dt;
    if (ShakeTime <= 0.0f)
      Effects->Shake = false;
  }
}

void Game::Render()
{
  // Render the game scene
  // This function should draw the game objects to the screen.;
  if (State == GAME_ACTIVE)
  {
    Effects->BeginRender();

    // Draw background
    Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
                         glm::vec2(static_cast<float>(Width), static_cast<float>(Height)));
    // Draw current level
    Levels[CurrentLevel].Draw(*Renderer);

    // Draw player paddle
    Player->Draw(*Renderer);

    // Draw Particles
    Particles->Draw();

    // Draw ball object
    Ball->Draw(*Renderer);

    // Draw PowerUps
    for (PowerUp &powerUp : PowerUps)
      if (!powerUp.Destroyed)
        powerUp.Draw(*Renderer);

    Effects->EndRender();
    Effects->Render(glfwGetTime());
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

    Ball->Stuck = Ball->Sticky;
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
      {
        block.Destroyed = true; // Mark block as destroyed
        SpawnPowerUps(block);

        if (Ball->PassThrough)
          continue;
      }
      else
      {
        // shake screen when hitting solid bricks
        ShakeTime = 0.05f;
        Effects->Shake = true;
      }

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

  // handle powerups
  for (PowerUp &powerUp : PowerUps)
  {
    if (!powerUp.Destroyed)
    {
      if (powerUp.Position.y >= Height)
        powerUp.Destroyed = true;
      if (CheckCollision(*Player, powerUp))
      { // collided with player, now activate powerup
        ActivatePowerUp(powerUp);
        powerUp.Destroyed = true;
        powerUp.Activated = true;
      }
    }
  }
}

void Game::SpawnPowerUps(GameObject &block)
{
  if (ShouldSpawn(75))
    PowerUps.push_back(
        PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("tex_speed")));
  if (ShouldSpawn(75))
    PowerUps.push_back(
        PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("tex_sticky")));
  if (ShouldSpawn(75))
    PowerUps.push_back(
        PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("tex_pass")));
  if (ShouldSpawn(75))
    PowerUps.push_back(
        PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("tex_increase")));
  if (ShouldSpawn(15))
    PowerUps.push_back(
        PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("tex_confuse")));
  if (ShouldSpawn(15))
    PowerUps.push_back(
        PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("tex_chaos")));
}

void Game::UpdatePowerUps(float dt)
{
  for (PowerUp &powerUp : PowerUps)
  {
    powerUp.Position += powerUp.Velocity * dt;
    if (powerUp.Activated)
    {
      powerUp.Duration -= dt;

      if (powerUp.Duration <= 0.0f)
      {
        // remove powerup from list (will later be removed)
        powerUp.Activated = false;
        // deactivate effects
        if (powerUp.Type == "sticky")
        {
          if (!IsOtherPowerUpActive(PowerUps, "sticky"))
          { // only reset if no other PowerUp of type sticky is active
            Ball->Sticky = false;
            Player->Color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "pass-through")
        {
          if (!IsOtherPowerUpActive(PowerUps, "pass-through"))
          { // only reset if no other PowerUp of type pass-through is active
            Ball->PassThrough = false;
            Ball->Color = glm::vec3(1.0f);
          }
        }
        else if (powerUp.Type == "confuse")
        {
          if (!IsOtherPowerUpActive(PowerUps, "confuse"))
          { // only reset if no other PowerUp of type confuse is active
            Effects->Confuse = false;
          }
        }
        else if (powerUp.Type == "chaos")
        {
          if (!IsOtherPowerUpActive(PowerUps, "chaos"))
          { // only reset if no other PowerUp of type chaos is active
            Effects->Chaos = false;
          }
        }
      }
    }
  }
  PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
                                [](const PowerUp &powerUp)
                                { return powerUp.Destroyed && !powerUp.Activated; }),
                 PowerUps.end());
}

/*
 * Private helper functions
 */

void Game::ResetLevel()
{
  // Could also just reload all the levels...
  // if (CurrentLevel == 0)
  //       Levels[0].Load("levels/one.lvl", Width, Height / 2);
  //   else if (CurrentLevel == 1)
  //       Levels[1].Load("levels/two.lvl", Width, Height / 2);
  //   else if (CurrentLevel == 2)
  //       Levels[2].Load("levels/three.lvl", Width, Height / 2);
  //   else if (CurrentLevel == 3)
  //       Levels[3].Load("levels/four.lvl", Width, Height / 2);
  for (GameObject &brick : Levels[CurrentLevel].Bricks)
  {
    brick.Destroyed = false;
  }
}

void Game::ResetPlayer()
{
  Player->Size = PLAYER_SIZE;
  Player->Position = glm::vec2(Width / 2.0f - (PLAYER_SIZE.x / 2.0f), Height - PLAYER_SIZE.y);
  Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f),
              INITIAL_BALL_VELOCITY);
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

bool Game::ShouldSpawn(unsigned int chance)
{
  unsigned int random = rand() % chance;
  return random == 0;
}

bool Game::IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
  for (const PowerUp &powerUp : powerUps)
  {
    if (powerUp.Activated)
      if (powerUp.Type == type)
        return true;
  }
  return false;
}

void Game::ActivatePowerUp(PowerUp &powerUp)
{
  // Positive PowerUps
  if (powerUp.Type == "speed")
  {
    Ball->Velocity *= 1.2;
  }
  else if (powerUp.Type == "sticky")
  {
    Ball->Sticky = true;
    Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
  }
  else if (powerUp.Type == "pass-through")
  {
    Ball->PassThrough = true;
    Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
  }
  else if (powerUp.Type == "pad-size-increase")
  {
    Player->Size.x += 50;
  }
  // Negative Powerups
  else if (powerUp.Type == "confuse")
  {
    if (!Effects->Confuse)
      Effects->Confuse = true;
  }
  else if (powerUp.Type == "chaos")
  {
    if (!Effects->Chaos)
      Effects->Chaos = true;
  }
}
