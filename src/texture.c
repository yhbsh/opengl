#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    const int window_width  = 800;
    const int window_height = 600;

    glfwInit();

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Animated UV Pattern", NULL, NULL);
    glfwMakeContextCurrent(window);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const int width  = 256;
    const int height = 256;
    uint8_t *data    = malloc(width * height * 3);

    while (!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (float)(x) / width;
                float v = (float)(y) / height;
                u += sin(time * 2.0f + v * 4.0f) * 0.1f;
                v += cos(time * 3.0f + u * 2.0f) * 0.1f;
                u = fmod(u, 1.0f);
                v = fmod(v, 1.0f);

                data[(y * width + x) * 3 + 0] = (uint8_t)(u * 255);
                data[(y * width + x) * 3 + 1] = (uint8_t)(v * 255);
                data[(y * width + x) * 3 + 2] = 0;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, window_height, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);

        // clang-format off
        glLoadIdentity();
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(window_width, 0);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(window_width, window_height);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(0, window_height);
          glEnd();
        glPopMatrix();
        // clang-format on

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(data);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
