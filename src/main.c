#include <SDL.h>
#include <stdlib.h>
#include <string.h>

#include "ks_version.h"

/*
 * Placeholder front end: opens the display and draws a test card until the player quits
 * (START on PSP, Escape or window close on desktop).
 *
 * Desktop only: "ks --frames N" renders N frames and exits, for smoke tests.
 */

static void draw_test_card(SDL_Renderer *renderer, unsigned frame) {
    static const SDL_Color bars[] = {
        {235, 235, 235, 255}, {235, 235, 16, 255}, {16, 235, 235, 255}, {16, 235, 16, 255},
        {235, 16, 235, 255},  {235, 16, 16, 255},  {16, 16, 235, 255},  {16, 16, 16, 255},
    };
    const int count = (int)(sizeof bars / sizeof bars[0]);
    const int bar_w = KS_SCREEN_W / count;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < count; i++) {
        SDL_Rect r = {i * bar_w, 0, bar_w, KS_SCREEN_H - 32};
        SDL_SetRenderDrawColor(renderer, bars[i].r, bars[i].g, bars[i].b, 255);
        SDL_RenderFillRect(renderer, &r);
    }

    /* A moving marker shows that the frame loop is alive. */
    SDL_Rect marker = {(int)(frame * 2 % (KS_SCREEN_W - 16)), KS_SCREEN_H - 24, 16, 16};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &marker);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    long frame_limit = -1;
    for (int i = 1; i + 1 < argc; i++) {
        if (strcmp(argv[i], "--frames") == 0) {
            frame_limit = strtol(argv[i + 1], NULL, 10);
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("Katawa Shoujo " KS_VERSION_STRING, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, KS_SCREEN_W, KS_SCREEN_H, 0);
    SDL_Renderer *renderer =
        window ? SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC) : NULL;
    if (!renderer) {
        SDL_Log("could not open the display: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GameController *pad = NULL;
    for (int i = 0; i < SDL_NumJoysticks() && !pad; i++) {
        if (SDL_IsGameController(i)) {
            pad = SDL_GameControllerOpen(i);
        }
    }

    int running = 1;
    unsigned frame = 0;
    while (running && (frame_limit < 0 || (long)frame < frame_limit)) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            } else if (event.type == SDL_CONTROLLERBUTTONDOWN &&
                       event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                running = 0;
            }
        }
        draw_test_card(renderer, frame++);
    }

    if (pad) {
        SDL_GameControllerClose(pad);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
