#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/OpenGL.h>
#include <stdio.h>

int main() {
  glfwInit();

  GLFWwindow *w = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(w);
  glfwSwapInterval(1);

  const GLubyte *version = glGetString(GL_VERSION);
  printf("version: %s\n", version);

  while (!glfwWindowShouldClose(w)) {
    glClearColor(0.1f, 0.5f, 0.5f, 1.0f);
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
