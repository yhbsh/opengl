#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>
#include <stdio.h>

int main() {
  glfwInit();
  GLFWwindow *w = glfwCreateWindow(600, 600, "w", glfwGetPrimaryMonitor(), NULL);
  glfwMakeContextCurrent(w);

  float pos[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};

  GLuint buff;
  glGenBuffers(1, &buff);
  glBindBuffer(GL_ARRAY_BUFFER, buff);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), pos, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  while (!glfwWindowShouldClose(w)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(w);
    glfwPollEvents();
  }

  return 0;
}
