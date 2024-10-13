# include "include/main.h"
# include <SDL2/SDL.h>
# include <stdio.h>

# define SCREEN_WIDTH 384
# define SCREEN_HEIGHT 216

# define MAP_SIZE 8
static u8 MAPDATA[MAP_SIZE * MAP_SIZE] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 2, 0, 4, 4, 0, 1,
    1, 0, 0, 0, 4, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

struct {
    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    struct { 
        Vec2 pos, dir, plane;
    } camera;

    Bool quit;
} state;

static void verline(int x, int y0, int y1, u32 color) {
    for (int y = y0; y <= y1; y++) {
        state.pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

static void render() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        // X coordinate in space from [-1, 1]
        const f32 xcam = (2 * (x / (f32) (SCREEN_WIDTH))) - 1;

        // Ray direction through this column
        const Vec2 dir = {
            state.camera.dir.x + state.camera.plane.x * xcam,
            state.camera.dir.y + state.camera.plane.y * xcam
        };
        
        Vec2 pos = state.camera.pos;
        Vec2i ipos = { (int) pos.x, (int) pos.y };
        
        // Distance ray must travel from one x/y side to the next
        const Vec2 deltadist = {
            fabsf(dir.x) < 1e-20 ? 1e30 : fabsf(1.0f / dir.x),
            fabsf(dir.y) < 1e-20 ? 1e30 : fabsf(1.0f / dir.y),
        };
        
        // Distance from start position to first x/y side
        Vec2 sidedist = {
            deltadist.x * (dir.x < 0 ? (pos.x - ipos.x) : (ipos.x + 1 - pos.x)),
            deltadist.y * (dir.y < 0 ? (pos.y - ipos.y) : (ipos.y + 1 - pos.y)),
        };
        
        // Integer step direction for x/y, calculated from overall diff
        const Vec2i step = { (int) sign(dir.x), (int) sign(dir.y) };
        
        // DDA hit
        struct { int val, side; Vec2 pos; } hit = { 0, 0, { 0.0f, 0.0f } };
        
        while (!hit.val) {
            if (sidedist.x < sidedist.y) {
                sidedist.x += deltadist.x;
                ipos.x += step.x;
                hit.side = 0;
            } else {
                sidedist.y += deltadist.y;
                ipos.y += step.y;
                hit.side = 1;
            }
            
            ASSERT(
                ipos.x >= 0
                && ipos.x < MAP_SIZE
                && ipos.y >= 0
                && ipos.y < MAP_SIZE,
                "DDA out of bounds");

            hit.val = MAPDATA[ipos.y * MAP_SIZE + ipos.x];
        }
        
        u32 color;
        
        switch (hit.val) {
            case 1: color = 0xFF0000FF; break;
            case 2: color = 0xFF00FF00; break;
            case 3: color = 0xFFFF0000; break;
            case 4: color = 0xFFFF00FF; break;
        }
        
        // Darken colors on y-sides
        if (hit.side == 1) {
            const u32
                br = ((color & 0xFF00FF) * 0xC0) >> 8,
                g  = ((color & 0x00FF00) * 0xC0) >> 8;
        
            color = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
        }
        
        hit.pos = (Vec2) { pos.x + sidedist.x, pos.y + sidedist.y };
        
        // Distance to hit
        const f32 dperp =
            hit.side == 0 ?
                (sidedist.x - deltadist.x)
                : (sidedist.y - deltadist.y);

        // Perform perspective division
        // Calculate line height relative to screen center
        const int
            h = (int) (SCREEN_HEIGHT / dperp),
            y0 = max((SCREEN_HEIGHT / 2) - (h / 2), 0),
            y1 = min((SCREEN_HEIGHT / 2) + (h / 2), SCREEN_HEIGHT - 1);

        verline(x, 0, y0, 0xFF202020);
        verline(x, y0, y1, color);
        verline(x, y1, SCREEN_HEIGHT - 1, 0xFF505050);
    }
}

static void rotate(f32 rot) {
    const Vec2 d = state.camera.dir, p = state.camera.plane;
    
    state.camera.dir.x = d.x * cos(rot) - d.y * sin(rot);
    state.camera.dir.y = d.x * sin(rot) + d.y * cos(rot);
    state.camera.plane.x = p.x * cos(rot) - p.y * sin(rot);
    state.camera.plane.y = p.x * sin(rot) + p.y * cos(rot);
}

int main(int argc, char *argv[]) {
    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "SDL failed to initialize %s\n",
        SDL_GetError());

    state.window =
        SDL_CreateWindow(
            "C DOOM",
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

    state.camera.pos = (Vec2) {2, 2};
    state.camera.dir = normalize(((Vec2) {-1.0f, 0.1f}));
    state.camera.plane = (Vec2) {0.0f, 0.66f};

    int mousex, mousey;
    SDL_ShowCursor(false);
    while (!state.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:   
                    state.quit = true;
                    break;
            }
        }

        const f32
            rotspeed = 1.5f * 0.016f,
            movespeed = 1.0f * 0.016f;

        const u8 *keystate = SDL_GetKeyboardState(NULL);
        
        SDL_GetMouseState(&mousex, &mousey);

        if (mousex < 1280 / 2)
            rotate(+rotspeed);
        
        if (mousex > 1280 / 2)
            rotate(-rotspeed);
        
        SDL_WarpMouseInWindow(state.window, 1280 / 2, 720 / 2);

        if (keystate[SDL_SCANCODE_W]) {
            state.camera.pos.x += state.camera.dir.x * movespeed;
            state.camera.pos.y += state.camera.dir.y * movespeed;
        }
        
        if (keystate[SDL_SCANCODE_S]) {
            state.camera.pos.x -= state.camera.dir.x * movespeed;
            state.camera.pos.y -= state.camera.dir.y * movespeed;
        }

        if (keystate[SDL_SCANCODE_ESCAPE]) {
            state.quit = true;
        }

        memset(state.pixels, 0, sizeof(state.pixels));
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

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}
