#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static void error_callback(int error, const char *description)
{
  fprintf(stderr, "[ERROR]: code: %d - error: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main()
{
  const int width = 500;
  const int height = 500;
  glfwSetErrorCallback(error_callback);
  glfwInit();
  GLFWwindow *w = glfwCreateWindow(width, height, "Video", NULL, NULL);
  glfwMakeContextCurrent(w);
  glfwSetKeyCallback(w, key_callback);
  glewInit();

  uint32_t *buffer = malloc(width * height * 4); // Correct buffer size

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      uint8_t red = ((float)x / width) * 255;
      uint8_t green = ((float)y / height) * 255;
      uint8_t blue = ((float)(y + x) / (width + height)) * 255;

      buffer[y * width + x] = (((uint32_t)red) << (8 * 2)) | (((uint32_t)green) << (8 * 1)) | ((uint32_t)blue << (8 * 0));
    }
  }

  while (!glfwWindowShouldClose(w))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glFlush();

    glfwSwapBuffers(w);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
