#include "SDL2/SDL_stdinc.h"
#include <SDL2/SDL.h>

int main()
{
  SDL_Window *window = NULL;
  SDL_Surface *surface = NULL;
  int width = 640;
  int height = 480;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  // Create a window
  window = SDL_CreateWindow("Pixel Buffer Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
  if (window == NULL)
  {
    printf("Window creation failed: %s\n", SDL_GetError());
    return 1;
  }

  // Create a surface (pixel buffer)
  surface = SDL_GetWindowSurface(window);

  // Fill the pixel buffer with a gradient pattern
  SDL_LockSurface(surface);
  Uint32 *pixels = (Uint32 *)surface->pixels;
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      Uint8 red = (Uint8)(((float)x / width) * 255);
      Uint8 green = (Uint8)(((float)y / height) * 255);
      Uint8 blue = (Uint8)(((float)(x + y) / (width + height)) * 255);
      pixels[y * width + x] = SDL_MapRGB(surface->format, red, green, blue);
    }
  }
  SDL_UnlockSurface(surface);

  // Update the window with the pixel buffer
  SDL_UpdateWindowSurface(window);

  SDL_bool exit = SDL_FALSE;
  SDL_Event eventData;
  while (!exit)
  {
    while (SDL_PollEvent(&eventData))
    {
      switch (eventData.type)
      {
      case SDL_QUIT:
        exit = SDL_TRUE;
        break;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
