#define GL_SILENCE_DEPRECATION  // Silence deprecated OpenGL warnings

#include <GLFW/glfw3.h>
#include <stdio.h>

#include <cmath>

int main() {
  const int window_width = 800;
  const int window_height = 600;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_SAMPLES, 4);  // Enable 4x multisampling

  GLFWwindow* window = glfwCreateWindow(
      window_width, window_height, "Animated UV Pattern", NULL, NULL);
  glfwMakeContextCurrent(window);
  glEnable(GL_MULTISAMPLE);  // Enable multisampling

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const int width = 256;
  const int height = 256;

  while (!glfwWindowShouldClose(window)) {
    float time = static_cast<float>(glfwGetTime());

    unsigned char* data = new unsigned char[width * height * 3];

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        float u = static_cast<float>(x) / width;
        float v = static_cast<float>(y) / height;

        u += sin(time * 2.0f + v * 4.0f) * 0.1f;
        v += cos(time * 3.0f + u * 2.0f) * 0.1f;

        u = fmod(u, 1.0f);
        v = fmod(v, 1.0f);

        data[(y * width + x) * 3 + 0] =
            static_cast<unsigned char>(u * 255);
        data[(y * width + x) * 3 + 1] =
            static_cast<unsigned char>(v * 255);
        data[(y * width + x) * 3 + 2] = 0;
      }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    delete[] data;

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(0, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(window_width, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(window_width, window_height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(0, window_height);
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}