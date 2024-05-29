#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

const char *vs = "#version 120\n"
                 "attribute vec4 position;\n"
                 "attribute vec2 texCoord;\n"
                 "varying vec2 texCoordVarying;\n"
                 "void main() {\n"
                 "    gl_Position = position;\n"
                 "    texCoordVarying = texCoord;\n"
                 "}\n";

const char *fs = "#version 120\n"
                 "varying vec2 texCoordVarying;\n"
                 "uniform sampler2D textureY;\n"
                 "uniform sampler2D textureU;\n"
                 "uniform sampler2D textureV;\n"
                 "void main() {\n"
                 "    vec3 yuv;\n"
                 "    vec3 rgb;\n"
                 "    yuv.x = texture2D(textureY, texCoordVarying).r;\n"
                 "    yuv.y = texture2D(textureU, texCoordVarying).r - 0.5;\n"
                 "    yuv.z = texture2D(textureV, texCoordVarying).r - 0.5;\n"
                 "    rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.344, 1.772, 1.402, -0.714, 0.0) * yuv;\n"
                 "    gl_FragColor = vec4(rgb, 1.0);\n"
                 "}\n";

int main() {
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(640, 480, "YUV420p to RGB", NULL, NULL);
    glfwMakeContextCurrent(window);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fs, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    int            width     = 640;
    int            height    = 480;
    unsigned char *yuvBuffer = (unsigned char *) malloc(width * height * 3 / 2);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            yuvBuffer[i * width + j] = (i + j) % 256;
        }
    }
    for (int i = 0; i < height / 2; i++) {
        for (int j = 0; j < width / 2; j++) {
            yuvBuffer[width * height + i * width / 2 + j]                      = (i + j) % 256;
            yuvBuffer[width * height + width * height / 4 + i * width / 2 + j] = (i + j) % 256;
        }
    }

    GLuint textureY, textureU, textureV;
    glGenTextures(1, &textureY);
    glGenTextures(1, &textureU);
    glGenTextures(1, &textureV);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureY"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer + width * height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureU"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer + width * height + width * height / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureV"), 2);

    // clang-format off
    GLfloat vertices[] = {
        -1.0f, -1.0f, +0.0f, +0.0f, 
        +1.0f, -1.0f, +1.0f, +0.0f, 
        -1.0f, +1.0f, +0.0f, +1.0f, 
        +1.0f, +1.0f, +1.0f, +1.0f
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &textureY);
    glDeleteTextures(1, &textureU);
    glDeleteTextures(1, &textureV);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(yuvBuffer);
    glfwTerminate();

    return 0;
}
