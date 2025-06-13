#include "particle_generator.hpp"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int nr_particles)
    : ParticleShader(shader), ParticleTex(texture)
{
  for (unsigned int i = 0; i < nr_particles; ++i)
    particles.push_back(Particle());

  initRenderData();
}

void ParticleGenerator::Update(float dt, GameObject &object,
                               unsigned int nr_new_particles, glm::vec2 offset)
{
  for (unsigned int i = 0; i < nr_new_particles; ++i)
  {
    int unusedParticle = FirstUnusedParticle();
    RespawnParticle(particles[unusedParticle], object, offset);
  }
  // update all particles
  for (unsigned int i = 0; i < particles.size(); ++i)
  {
    Particle &p = particles[i];
    p.Life -= dt; // reduce life
    if (p.Life > 0.0f)
    { // particle is alive, thus update
      p.Position -= p.Velocity * dt;
      p.Color.a -= dt * 2.5f;
    }
  }
}

void ParticleGenerator::Draw()
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  ParticleShader.Use();
  for (Particle particle : particles)
  {
    if (particle.Life > 0.0f)
    {
      ParticleShader.SetVector2f("offset", particle.Position);
      ParticleShader.SetVector4f("color", particle.Color);
      ParticleTex.Bind();
      glBindVertexArray(particleVAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * Private Helper Methods
 */
unsigned int ParticleGenerator::FirstUnusedParticle()
{
  // search from last used particle, this will usually return almost instantly
  for (unsigned int i = lastUsedParticle; i < particles.size(); ++i)
  {
    if (particles[i].Life <= 0.0f)
    {
      lastUsedParticle = i;
      return i;
    }
  }
  // otherwise, do a linear search
  for (unsigned int i = 0; i < lastUsedParticle; ++i)
  {
    if (particles[i].Life <= 0.0f)
    {
      lastUsedParticle = i;
      return i;
    }
  }
  // override first particle if all others are alive
  lastUsedParticle = 0;
  return 0;
}

void ParticleGenerator::RespawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
  float random = ((rand() % 100) - 50) / 10.0f;
  float rColor = 0.5f + ((rand() % 100) / 100.0f);
  particle.Position = object.Position + random + offset;
  particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
  particle.Life = 1.0f;
  particle.Velocity = object.Velocity * 0.1f;
}

void ParticleGenerator::initRenderData()
{
  // Configure VAO/VBO for particle rendering
  unsigned int VBO;
  float vertices[] = {
      // pos      // tex
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &particleVAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(particleVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position + texture attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}