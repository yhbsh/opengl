#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_WIDTH 50
#define GRID_HEIGHT 50
#define CELL_SIZE 10

int grid[GRID_WIDTH][GRID_HEIGHT];
int new_grid[GRID_WIDTH][GRID_HEIGHT];

void initialize_grid() {
    srand(time(NULL));
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y] = rand() % 2;
        }
    }
}

void draw_cell(int x, int y) {
    if (grid[x][y] == 1) {
        glBegin(GL_QUADS);
        glVertex2f(x * CELL_SIZE, y * CELL_SIZE);
        glVertex2f(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE);
        glVertex2f(x * CELL_SIZE + CELL_SIZE, y * CELL_SIZE + CELL_SIZE);
        glVertex2f(x * CELL_SIZE, y * CELL_SIZE + CELL_SIZE);
        glEnd();
    }
}
void update_grid() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            int live_neighbors = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;
                    int nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
                    int ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
                    live_neighbors += grid[nx][ny];
                }
            }
            if (grid[x][y] == 1 && (live_neighbors < 2 || live_neighbors > 3)) {
                new_grid[x][y] = 0;
            } else if (grid[x][y] == 0 && live_neighbors == 3) {
                new_grid[x][y] = 1;
            } else {
                new_grid[x][y] = grid[x][y];
            }
        }
    }
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y] = new_grid[x][y];
        }
    }
}
void render_grid() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            draw_cell(x, y);
        }
    }
}
int main(void) {
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, "Game of Life", NULL, NULL);
    glfwMakeContextCurrent(window);
    glOrtho(0, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, 0, -1, 1);

    initialize_grid();

    while (!glfwWindowShouldClose(window)) {
        update_grid();
        render_grid();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
