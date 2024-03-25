#define GL_SILENCE_DEPRECATION  // Silence deprecated OpenGL warnings
#include <GLFW/glfw3.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "[USAGE]: ./main [url]");
    return 1;
  }

  int ret;
  const int width = 800;
  const int height = 600;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  GLFWwindow *window = glfwCreateWindow(width, height, "Animated UV Pattern", NULL, NULL);
  glfwMakeContextCurrent(window);
  printf("version = %s\n", glGetString(GL_VERSION));

  AVFormatContext *format_context = NULL;
  ret = avformat_open_input(&format_context, argv[1], NULL, NULL);
  ret = avformat_find_stream_info(format_context, NULL);
  AVStream *stream = format_context->streams[1];
  enum AVCodecID codec_id = stream->codecpar->codec_id;
  const AVCodec *codec = avcodec_find_decoder(codec_id);
  AVCodecContext *codec_context = avcodec_alloc_context3(codec);
  ret = avcodec_parameters_to_context(codec_context, stream->codecpar);
  ret = avcodec_open2(codec_context, codec, NULL);
  AVFrame *frame = av_frame_alloc();
  AVPacket packet;

  struct SwsContext *sws_context = NULL;

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  while (!glfwWindowShouldClose(window)) {
    ret = av_read_frame(format_context, &packet);
    if (packet.stream_index != stream->index) {
      continue;
    }

    ret = avcodec_send_packet(codec_context, &packet);
    while (ret >= 0) {
      ret = avcodec_receive_frame(codec_context, frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        break;
      }

      AVFrame *output_frame = av_frame_alloc();
      struct SwsContext *sws_context =
          sws_getContext(frame->width, frame->height, frame->format, width, height,
                         AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

      sws_scale_frame(sws_context, output_frame, frame);

      sws_freeContext(sws_context);

      glClear(GL_COLOR_BUFFER_BIT);

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                   output_frame->data[0]);

      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, width, height, 0, -1, 1);

      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2i(0, 0);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2i(width, 0);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2i(width, height);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2i(0, height);
      glEnd();

      glfwSwapBuffers(window);
      glfwPollEvents();

      av_frame_free(&output_frame);
    }
    av_packet_unref(&packet);
  }

  glfwTerminate();
  return 0;
}
