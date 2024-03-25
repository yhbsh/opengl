#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/GL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>

static void error_callback(int error, const char *description) {
  fprintf(stderr, "[ERROR]: code: %d - error: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main() {
  int ret;

  const int width = 960;
  const int height = 540;
  glfwSetErrorCallback(error_callback);
  glfwInit();

  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *w = glfwCreateWindow(width, height, "Video", NULL, NULL);
  glfwMakeContextCurrent(w);
  glfwSetKeyCallback(w, key_callback);

  printf("version = %s\n", glGetString(GL_VERSION));

  AVFormatContext *format_context = NULL;
  ret = avformat_open_input(&format_context,
                            "rtmp://localhost:1935/live/stream", NULL, NULL);
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
  ret = avcodec_parameters_to_context(codec_context, stream->codecpar);
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

  while (!glfwWindowShouldClose(w)) {
    ret = av_read_frame(format_context, &packet);
    if (ret < 0) {
      fprintf(stderr, "[ERROR]: av_read_frame(): %s\n", av_err2str(ret));
      return 1;
    }
    if (packet.stream_index != stream->index) {
      continue;
    }

    ret = avcodec_send_packet(codec_context, &packet);
    if (ret < 0) {
      fprintf(stderr, "[ERROR]: avcodec_send_packet(): %s\n", av_err2str(ret));
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

      const char *name = av_get_pix_fmt_name(frame->format);
      printf("format: %s - width: %d - height: %d - linesize: %d\n", name,
             frame->width, frame->height, frame->linesize[0]);

      AVFrame *output_frame = av_frame_alloc();
      struct SwsContext *sws_context = sws_getContext(
          frame->width, frame->height, frame->format, width, height,
          AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

      sws_scale_frame(sws_context, output_frame, frame);

      sws_freeContext(sws_context);

      uint8_t *src_data = output_frame->data[0];
      int src_stride = output_frame->linesize[0];
      int dst_stride = output_frame->linesize[0];

      int buffer_size = output_frame->linesize[0] * output_frame->height;
      uint8_t *flipped_data = (uint8_t *)malloc(buffer_size);
      for (int y = 0; y < output_frame->height; ++y) {
        memcpy(flipped_data + (output_frame->height - 1 - y) * dst_stride,
               src_data + y * src_stride, src_stride);
      }

      glClear(GL_COLOR_BUFFER_BIT);
      glDrawPixels(output_frame->width, output_frame->height, GL_RGB,
                   GL_UNSIGNED_BYTE, flipped_data);
      free(flipped_data);

      glViewport(0, 0, width, height);

      glfwSwapBuffers(w);
      glfwPollEvents();
    }

    av_packet_unref(&packet);
  }

  glfwTerminate();
  return 0;
}
