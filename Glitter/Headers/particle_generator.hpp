#ifndef PARTICLEGENERATOR_HPP
#define PARTICLEGENERATOR_HPP

#include <glm/glm.hpp>

#include "game_object.hpp"
#include "shader.hpp"

struct Particle
{
  glm::vec2 Position, Velocity;
  glm::vec4 Color;
  float Life;

  Particle()
      : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator
{
public:
  ParticleGenerator(Shader shader, Texture2D texture, unsigned int nr_particles);

  void Update(float dt, GameObject &object, unsigned int nr_new_particles, glm::vec2 offset);
  void Draw();

private:
  Shader ParticleShader;
  Texture2D ParticleTex;

  unsigned int particleVAO;

  // unsigned int nr_particles = 500;
  unsigned int lastUsedParticle = 0;
  std::vector<Particle> particles;

  unsigned int FirstUnusedParticle();
  void RespawnParticle(Particle &particle, GameObject &object, glm::vec2 offset);
  void initRenderData();
};

#endif