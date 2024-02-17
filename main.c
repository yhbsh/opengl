#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>
#include <stdio.h>

int main() {
  glfwInit();
  GLFWwindow *w = glfwCreateWindow(800, 600, "Hello World", glfwGetPrimaryMonitor(), NULL);

  glfwMakeContextCurrent(w);

  printf("version: %s\n", glGetString(GL_VERSION));

  while (!glfwWindowShouldClose(w)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    glfwSwapBuffers(w);
    glfwPollEvents();
  }

  glfwDestroyWindow(w);
  glfwTerminate();

  return 0;
}
