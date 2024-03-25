#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEADER_SIZE 100

GLuint texture;
int image_width, image_height;
unsigned char *image_data;

void load_ppm(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return;
  }

  char header[MAX_HEADER_SIZE];
  fgets(header, MAX_HEADER_SIZE, fp);
  if (strcmp(header, "P6\n")) {
    fprintf(stderr, "Invalid PPM file format\n");
    fclose(fp);
    return;
  }

  int maxval;
  fscanf(fp, "%d %d %d\n", &image_width, &image_height, &maxval);

  image_data = (unsigned char *)malloc(image_width * image_height * 3);
  fread(image_data, 3, image_width * image_height, fp);

  fclose(fp);
}

void init_opengl() {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image_data);

  GLint width, height;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
  printf("Texture dimensions: %d x %d\n", width, height);

  free(image_data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.5, 1.0); // Set clear color to red

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-1.0f, -1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(1.0f, -1.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(1.0f, 1.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(-1.0f, 1.0f);
  glEnd();
}

int main() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = glfwCreateWindow(600, 600, "Texture", NULL, NULL);

  glfwMakeContextCurrent(window);
  printf("version = %s\n", glGetString(GL_VERSION));

  glewExperimental = GL_TRUE;
  glewInit();

  load_ppm("file.ppm");
  init_opengl();

  while (!glfwWindowShouldClose(window)) {
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}