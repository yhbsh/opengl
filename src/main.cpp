#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>  // Include the GLFW library for window creation and input handling
#include <stdio.h>  // Include the standard input/output library

int main() {
  // Define the window dimensions
  const int window_width = 800;
  const int window_height = 600;

  // Initialize GLFW
  glfwInit();

  // Set the OpenGL version to 2.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // Create a window with the specified dimensions and title
  GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                        "UV Pattern", NULL, NULL);

  // Make the OpenGL context of the window current
  glfwMakeContextCurrent(window);

  // Create a texture object
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set the texture wrapping and filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Define the texture dimensions
  const int width = 256;
  const int height = 256;

  // Allocate memory for the texture data
  unsigned char* data = new unsigned char[width * height * 3];

  // Generate the UV pattern texture data
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // Set the red channel to the U coordinate (0 to 255)
      data[(y * width + x) * 3 + 0] = static_cast<unsigned char>(x);
      // Set the green channel to the V coordinate (0 to 255)
      data[(y * width + x) * 3 + 1] = static_cast<unsigned char>(y);
      // Set the blue channel to 0
      data[(y * width + x) * 3 + 2] = 0;
    }
  }

  // Upload the texture data to the GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);

  // Free the allocated memory for the texture data
  delete[] data;

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
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

  // Terminate GLFW
  glfwTerminate();

  return 0;
}