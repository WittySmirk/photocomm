#include "shared.h"

void renderText(SDL_Renderer* renderer, TTF_Font* font, char *text, int x, int y, bool center, SDL_Color color) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // Query dimensions from the texture, not the freed surface
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect dst = {};
        if (center) {
            dst = (SDL_Rect){.x = (SCREEN_W / 2) - (w / 2), .y = (SCREEN_H / 2) - (h / 2), .w = w, .h = h};
        } else {
            dst = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};
        }

        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
}
