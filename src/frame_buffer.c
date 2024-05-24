#include <SDL2/SDL.h>

int main() {
    SDL_Window  *window  = NULL;
    SDL_Surface *surface = NULL;
    size_t       width   = 640;
    size_t       height  = 480;

    SDL_Init(SDL_INIT_VIDEO);
    window  = SDL_CreateWindow("Pixel Buffer Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);

    SDL_LockSurface(surface);
    uint32_t *pixels = (uint32_t *) surface->pixels;

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            uint8_t red           = (x / width) * 255;
            uint8_t green         = (y / height) * 255;
            uint8_t blue          = ((x + y) / (width + height)) * 255;
            pixels[y * width + x] = SDL_MapRGB(surface->format, red, green, blue);
        }
    }
    SDL_UnlockSurface(surface);

    SDL_UpdateWindowSurface(window);

    SDL_bool  quit = SDL_FALSE;
    SDL_Event eventData;
    while (!quit) {
        while (SDL_PollEvent(&eventData)) {
            switch (eventData.type) {
                case SDL_QUIT:
                    quit = SDL_TRUE;
                    break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
