#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <cmath>  // Include the math library for sine and cosine functions

int main() {
  const int window_width = 800;
  const int window_height = 600;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_SAMPLES, 16);

  GLFWwindow* window =
      glfwCreateWindow(window_width, window_height, "Animated UV Pattern", NULL, NULL);
  glfwMakeContextCurrent(window);

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
    // Get the current time in seconds
    float time = static_cast<float>(glfwGetTime());

    // Allocate memory for the texture data
    unsigned char* data = new unsigned char[width * height * 3];

    // Generate the animated UV pattern texture data
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        // Calculate the U and V coordinates based on the current time
        float u = static_cast<float>(x) / width;
        float v = static_cast<float>(y) / height;

        // Apply a sine wave pattern to the U coordinate
        u += sin(time * 2.0f + v * 4.0f) * 0.1f;

        // Apply a cosine wave pattern to the V coordinate
        v += cos(time * 3.0f + u * 2.0f) * 0.1f;

        // Clamp the U and V coordinates to the range [0, 1]
        u = fmod(u, 1.0f);
        v = fmod(v, 1.0f);

        // Set the color channels based on the modified U and V
        // coordinates
        data[(y * width + x) * 3 + 0] = static_cast<unsigned char>(u * 255);
        data[(y * width + x) * 3 + 1] = static_cast<unsigned char>(v * 255);
        data[(y * width + x) * 3 + 2] = 0;
      }
    }

    // Upload the texture data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Free the allocated memory for the texture data
    delete[] data;

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, -1, 1);

    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Begin rendering a quad
    glBegin(GL_QUADS);
    // Set the texture coordinates and vertex positions for each
    // corner of the quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(0, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(window_width, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(window_width, window_height);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(0, window_height);
    glEnd();

    // Swap the front and back buffers
    glfwSwapBuffers(window);

    // Poll for events
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}