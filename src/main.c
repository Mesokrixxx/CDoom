# include "include/main.h"
# include <SDL2/SDL.h>
# include <stdint.h>
# include <stdio.h>

# define SCREEN_WIDTH 320
# define SCREEN_HEIGHT 180

static int MAPDATA[8 * 8] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 4, 4, 0, 1,
    1, 0, 0, 0, 0, 4, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 2, 2, 0, 3, 3, 1,
    1, 0, 2, 2, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

struct {
    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    unsigned int pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    struct { 
        Vec2 pos, dir, plane;
    } camera;

    Bool quit;
} state;

static void verline(int x, int y0, int y1, unsigned int color) {
    for (int y = y0; y < y1; y++)
        state.pixels[(y * SCREEN_WIDTH) + x] = color;
}

static void render() {
    for (int x = 0; x < SCREEN_WIDTH; x++)
        verline(x, 20, SCREEN_HEIGHT - 20, 0xFFFF0000 | (x & 0xFF));
}

int main(int argc, char *argv[]) {
    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "SDL failed to initialize %s\n",
        SDL_GetError());

    state.window =
        SDL_CreateWindow(
            "Test",
            SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
            1280, 720,
            SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(state.window, "Failed to create SDL window: %s\n", SDL_GetError());

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer, "Failed to create SDL renderer: %s\n", SDL_GetError());

    state.texture = 
        SDL_CreateTexture(
            state.renderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);
    ASSERT(state.texture, "Failed to create SDL texture: %s\n", SDL_GetError());

    while (!state.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:   
                    state.quit = true;
                    break;
            }
        }

        render();

        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL, NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyWindow(state.window);
    return 0;
}
