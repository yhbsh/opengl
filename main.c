#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
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

int main(void)
{
  glfwSetErrorCallback(error_callback);
  glfwInit();

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *w = glfwCreateWindow(600, 600, "Rotating Rectangle", NULL, NULL);
  glfwMakeContextCurrent(w);
  glfwSetKeyCallback(w, key_callback);

  glewExperimental = GL_TRUE;
  GLenum error = glewInit();
  if (GLEW_OK != error)
  {
    fprintf(stderr, "[ERROR]: %s\n", glewGetErrorString(error));
    return 1;
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float points[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  const char *vertex_shader = "#version 400\n"
                              "uniform float rotation;"
                              "in vec2 vp;"
                              "void main() {"
                              "    float x = vp.x * cos(rotation) - vp.y * sin(rotation);"
                              "    float y = vp.x * sin(rotation) + vp.y * cos(rotation);"
                              "    gl_Position = vec4(x, y, 0.0, 1.0);"
                              "}";

  const char *fragment_shader = "#version 400\n"
                                "out vec4 frag_colour;"
                                "void main() {"
                                "    frag_colour = vec4(1.0, 0.5, 0.5, 1.0);"
                                "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, fs);
  glAttachShader(program, vs);
  glLinkProgram(program);
  glUseProgram(program);

  GLint rotation_loc = glGetUniformLocation(program, "rotation");

  while (!glfwWindowShouldClose(w))
  {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 600, 600);

    float rotation_angle = (float)glfwGetTime() * 12.0f;
    glUniform1f(rotation_loc, rotation_angle);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwPollEvents();
    glfwSwapBuffers(w);
  }

  glDeleteProgram(program);
  glDeleteShader(fs);
  glDeleteShader(vs);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwTerminate();
  return 0;
}
