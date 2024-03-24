#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x264.h>

#define INBUF_SIZE 4096

static void display(GLFWwindow *window, GLuint tex_handle, int width, int height)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, tex_handle);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(-1.0f, -1.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(1.0f, -1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(1.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-1.0f, 1.0f);
  glEnd();
  glfwSwapBuffers(window);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
    return -1;
  }

  FILE *infile = fopen(argv[1], "rb");
  if (!infile)
  {
    fprintf(stderr, "Could not open input file.\n");
    return -1;
  }

  GLFWwindow *window;
  if (!glfwInit())
  {
    fprintf(stderr, "Could not initialize GLFW.\n");
    return -1;
  }

  window = glfwCreateWindow(640, 480, "x264 OpenGL Video Player", NULL, NULL);
  if (!window)
  {
    fprintf(stderr, "Could not create GLFW window.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Could not initialize GLEW.\n");
    glfwTerminate();
    return -1;
  }

  x264_picture_t pic_in, pic_out;
  x264_param_t param;
  x264_t *encoder;
  x264_nal_t *nals;
  int num_nals, frame_size, i_frame = 0;
  int width, height;
  char buf[INBUF_SIZE];

  x264_param_default(&param);
  param.i_width = 640;
  param.i_height = 480;
  param.i_csp = X264_CSP_I420;
  param.i_log_level = X264_LOG_NONE;
  param.b_repeat_headers = 1;
  encoder = x264_encoder_open(&param);

  x264_picture_alloc(&pic_in, param.i_csp, param.i_width, param.i_height);
  width = param.i_width;
  height = param.i_height;

  GLuint tex_handle;
  glGenTextures(1, &tex_handle);
  glBindTexture(GL_TEXTURE_2D, tex_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  while (!glfwWindowShouldClose(window))
  {
    while (fgets(buf, INBUF_SIZE, infile))
    {
      if (!strncmp(buf, "FRAME", 5))
      {
        fread(&frame_size, 1, 4, infile);
        fread(pic_in.img.plane[0], 1, frame_size, infile);
        x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);

        for (int i = 0; i < num_nals; i++)
        {
          if (nals[i].i_type == NAL_SLICE)
          {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pic_out.img.plane[0]);
            display(window, tex_handle, width, height);
            i_frame++;
            break;
          }
        }
      }
    }

    glfwPollEvents();
  }

  x264_encoder_close(encoder);
  x264_picture_clean(&pic_in);
  glDeleteTextures(1, &tex_handle);
  glfwDestroyWindow(window);
  glfwTerminate();
  fclose(infile);

  return 0;
}
