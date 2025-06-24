#include <iostream>

#include "post_processor.hpp"

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
    : PostProcessingShader(shader), Texture(),
      Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
  glGenFramebuffers(1, &MSFBO);
  glGenFramebuffers(1, &FBO);
  glGenRenderbuffers(1, &RBO);

  glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  Texture.Generate(width, height, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.ID, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  initRenderData();
  PostProcessingShader.SetInteger("scene", 0, true);

  float offset = 1.0f / 300.0f;
  float offsets[9][2] = {
      {-offset, offset},
      {0.0f, offset},
      {offset, offset},
      {-offset, 0.0f},
      {0.0f, 0.0f},
      {offset, 0.0f},
      {-offset, -offset},
      {0.0f, -offset},
      {offset, -offset}};
  glUniform2fv(glGetUniformLocation(PostProcessingShader.ID, "offsets"), 9, (float *)offsets);

  int edge_kernel[9] = {
      -1, -1, -1,
      -1, 8, -1,
      -1, -1, -1};
  glUniform1iv(glGetUniformLocation(PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

  float blur_kernel[9] = {
      1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
      2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
      1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f};
  glUniform1fv(glGetUniformLocation(PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender()
{
  glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
  glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time)
{
  PostProcessingShader.Use();
  PostProcessingShader.SetFloat("time", time);
  PostProcessingShader.SetInteger("confuse", Confuse);
  PostProcessingShader.SetInteger("chaos", Chaos);
  PostProcessingShader.SetInteger("shake", Shake);

  glActiveTexture(GL_TEXTURE0);
  Texture.Bind();
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
  unsigned int VBO;

  float vertices[] = {
      // pos        // tex
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 0.0f, 1.0f,

      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f};

  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}