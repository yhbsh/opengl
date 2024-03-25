#include "libavutil/pixdesc.h"
#define GL_SILENCE_DEPRECATION  // Silence deprecated OpenGL warnings
#include <GLFW/glfw3.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int ret;
  const int window_width = 800;
  const int window_height = 600;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_SAMPLES, 4);  // Enable 4x multisampling

  GLFWwindow *window = glfwCreateWindow(
      window_width, window_height, "Animated UV Pattern", NULL, NULL);
  glfwMakeContextCurrent(window);
  glEnable(GL_MULTISAMPLE);  // Enable multisampling

  AVFormatContext *format_context = NULL;
  ret = avformat_open_input(&format_context,
                            "rtmp://localhost:1935/live/stream", NULL,
                            NULL);
  if (ret < 0) {
    fprintf(stderr, "[ERROR]: %s\n", av_err2str(ret));
    return 1;
  }

  ret = avformat_find_stream_info(format_context, NULL);
  if (ret < 0) {
    fprintf(stderr, "[ERROR]: %s\n", av_err2str(ret));
    return 1;
  }

  int vs = -1;
  AVStream *stream = NULL;
  for (int i = 0; i < format_context->nb_streams; i++) {
    stream = format_context->streams[i];
    if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      vs = i;
      break;
    }
  }
  if (vs == -1 || stream == NULL) {
    fprintf(stderr, "[ERROR]: did not find video stream\n");
    return 1;
  }

  enum AVCodecID codec_id = stream->codecpar->codec_id;
  const AVCodec *codec = avcodec_find_decoder(codec_id);
  if (codec == NULL) {
    fprintf(stderr, "[ERROR]: not codec found\n");
    return 1;
  }

  AVCodecContext *codec_context = avcodec_alloc_context3(codec);
  if (codec_context == NULL) {
    fprintf(stderr, "[ERROR]: could not allocate codec context\n");
    return 1;
  }
  ret =
      avcodec_parameters_to_context(codec_context, stream->codecpar);
  if (ret < 0) {
    fprintf(stderr, "[ERROR]: %s\n", av_err2str(ret));
    return 1;
  }

  ret = avcodec_open2(codec_context, codec, NULL);
  if (ret < 0) {
    fprintf(stderr, "[ERROR]: %s\n", av_err2str(ret));
    return 1;
  }

  AVFrame *frame = av_frame_alloc();
  if (frame == NULL) {
    fprintf(stderr, "[ERROR]: could not alloca frame\n");
    return 1;
  }
  AVPacket packet;

  struct SwsContext *sws_context = NULL;

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
    ret = av_read_frame(format_context, &packet);
    if (ret < 0) {
      fprintf(stderr, "[ERROR]: av_read_frame(): %s\n",
              av_err2str(ret));
      return 1;
    }
    if (packet.stream_index != stream->index) {
      continue;
    }

    ret = avcodec_send_packet(codec_context, &packet);
    if (ret < 0) {
      fprintf(stderr, "[ERROR]: avcodec_send_packet(): %s\n",
              av_err2str(ret));
      break;
    }

    while (ret >= 0) {
      ret = avcodec_receive_frame(codec_context, frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        break;
      } else if (ret < 0) {
        fprintf(stderr, "[ERROR]: avcodec_receive_frame(): %s\n",
                av_err2str(ret));
        break;
      }

      const char *name = av_get_pix_fmt_name(codec_context->pix_fmt);
      printf("format: %s - width: %d - height: %d - linesize: %d\n",
             name, frame->width, frame->height, frame->linesize[0]);

      AVFrame *output_frame = av_frame_alloc();
      struct SwsContext *sws_context = sws_getContext(
          frame->width, frame->height, frame->format, width, height,
          AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

      sws_scale_frame(sws_context, output_frame, frame);

      sws_freeContext(sws_context);

      uint8_t *src_data = output_frame->data[0];
      int src_stride = output_frame->linesize[0];
      int dst_stride = output_frame->linesize[0];

      int buffer_size =
          output_frame->linesize[0] * output_frame->height;
      uint8_t *flipped_data = (uint8_t *)malloc(buffer_size);
      for (int y = 0; y < output_frame->height; ++y) {
        memcpy(flipped_data +
                   (output_frame->height - 1 - y) * dst_stride,
               src_data + y * src_stride, src_stride);
      }

      glClear(GL_COLOR_BUFFER_BIT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, flipped_data);
      glDrawPixels(output_frame->width, output_frame->height, GL_RGB,
                   GL_UNSIGNED_BYTE, flipped_data);
      free(flipped_data);

      glClear(GL_COLOR_BUFFER_BIT);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, window_width, window_height, 0, -1, 1);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2i(0, 0);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2i(window_width, 0);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2i(window_width, window_height);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2i(0, window_height);
      glEnd();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}