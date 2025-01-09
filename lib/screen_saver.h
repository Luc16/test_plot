//
// Created by luc on 09/01/25.
//

#ifndef TEST_PLOT_SCREEN_SAVER_H
#define TEST_PLOT_SCREEN_SAVER_H

#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>

namespace iglp {

  class ScreenSaver {
  public:
    ScreenSaver() = default;
    ~ScreenSaver();

    static void saveFrameBufferToPNG(GLuint fbo, int width, int height, const char* filename);
    void startRecording(const char* filename, int width, int height);
    void endRecording();
    void saveCurrentWindowToPNG(const char* filename);
    void recordFrame(GLuint fbo, int width, int height);

   private:
    FILE* m_ffmpegFile = nullptr;
    bool created_frame_buffer = false;
    GLuint m_fbo{};
    GLuint m_tex{};
    GLuint m_rbo{};


  };

} // namespace iglp

#endif //TEST_PLOT_SCREEN_SAVER_H
