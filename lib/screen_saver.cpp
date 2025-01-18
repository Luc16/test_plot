//
// Created by luc on 09/01/25.
//

#include "screen_saver.hpp"

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace iglp {

ScreenSaver::~ScreenSaver() {
  if (created_frame_buffer) {
//    if (glIsFramebuffer(m_fbo)) {
//      glDeleteFramebuffers(1, &m_fbo);
//    }
//    if (glIsTexture(m_tex)) {
//      glDeleteTextures(1, &m_tex);
//    }
//    if (glIsRenderbuffer(m_rbo)) {
//      glDeleteRenderbuffers(1, &m_rbo);
//    }
  }
  if (m_ffmpegFile != nullptr)
    pclose(m_ffmpegFile);
}

void ScreenSaver::saveFrameBufferToPNG(GLuint fbo, int width, int height, const char* filename) {
  // Allocate memory to store the pixel data
//  auto start = std::chrono::high_resolution_clock::now();
  std::vector<unsigned char> pixels(width * height * 4);

  // Bind the framebuffer and read the pixel data
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  std::cout << "Time to read pixels: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;

//  start = std::chrono::high_resolution_clock::now();
  stbi_flip_vertically_on_write(true);
  // Save the pixel data to a PNG file
  stbi_write_png(filename, width, height, 4, pixels.data(), width * 4);

//  std::cout << "Time to write PNG: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
}

void ScreenSaver::saveCurrentWindowToPNG(const char* filename) {

  int width  = (int)ImGui::GetWindowSize().x;
  int height = (int)ImGui::GetWindowSize().y;

  if (!created_frame_buffer) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);


    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    created_frame_buffer = true;
  }

  glViewport(0, 0, width, height);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glBindTexture(GL_TEXTURE_2D, m_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  auto color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
  glClearColor(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  auto dl = ImGui::GetWindowDrawList();

  static ImDrawData drawData = ImDrawData();
  drawData.Clear();
  drawData.Valid = true;
  drawData.DisplayPos = ImGui::GetWindowPos();
  drawData.DisplaySize = ImGui::GetWindowSize();
  drawData.FramebufferScale = ImVec2(1.0f, 1.0f);
  drawData.AddDrawList(dl);


  ImGui_ImplOpenGL3_RenderDrawData(&drawData);


  saveFrameBufferToPNG(m_fbo, width, height, filename);

}

void ScreenSaver::startRecording(const char* filename, int width, int height) {
  // Allocate memory to store the pixel data
  width += width % 2;
  height += height % 2;

  std::stringstream ss;
  ss << "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s " << width << "x" << height << " -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " << filename;
  auto cmd = ss.str();

// open pipe to ffmpeg's stdin in binary write mode
  assert(m_ffmpegFile == nullptr);
  m_ffmpegFile = popen(cmd.c_str(), "w");
  if (m_ffmpegFile == nullptr) {
    std::cerr << "Error opening ffmpeg pipe" << std::endl;
    exit(1);
  }
}

void ScreenSaver::endRecording() {
  assert(m_ffmpegFile != nullptr);
  pclose(m_ffmpegFile);
  m_ffmpegFile = nullptr;
}

void ScreenSaver::recordFrame(GLuint fbo, int width, int height) {
  // Allocate memory to store the pixel data
//  auto start = std::chrono::high_resolution_clock::now();
  std::vector<unsigned char> pixels(width * height * 4);

  // Bind the framebuffer and read the pixel data
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  std::cout << "Writing frame to: " << m_ffmpegFile  << std::endl;

  // Write the pixel data to the ffmpeg pipe
  fwrite(pixels.data(), sizeof(unsigned char)*pixels.size(), 1, m_ffmpegFile);
//  std::cout << "Time to read pixels: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms" << std::endl;
}

} // namespace iglp
