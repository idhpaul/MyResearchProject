#include "Cursor.h"

#include <iostream>

#define TEST_SDL 1
#if TEST_SDL
#include "SDL.h"
#include <vector>
extern std::vector<uint8_t> image;
#endif



int main()
{
#if TEST_SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* surface = NULL;
    SDL_Cursor* cursor = NULL;
    SDL_bool error = SDL_TRUE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        goto exit;
    }
    if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) < 0) {
        goto exit;
    }
    /*surface = SDL_LoadBMP((1 < argc) ? argv[1] : "cursor.bmp");
    if (!surface) {
        goto exit;
    }*/
    /*cursor = SDL_CreateColorCursor(surface, 0, 0);
    if (!cursor) {
        goto exit;
    }*/
#endif


#if TEST_SDL
    //SDL_SetCursor(cursor);
    
    while (SDL_TRUE)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
            case SDL_MOUSEBUTTONUP:
            case SDL_QUIT:
                error = SDL_FALSE;
                goto exit;
            }
        }

        CursorInfo();
        
        /*SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB32);
        SDL_Surface* output = SDL_ConvertSurface(input, format, 0);
        SDL_FreeFormat(format);*/

        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)image.data(), 32, 32, 32, 32*4, SDL_PIXELFORMAT_ARGB32);

        cursor = SDL_CreateColorCursor(surface, 0, 0);
        if (!cursor) {
            goto exit;
        }
        SDL_SetCursor(cursor);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        Sleep(1000);
    }
#else
    while (true)
    {
        CursorInfo();
        Sleep(1000);
    }
#endif


	

#if TEST_SDL
    exit :
    if (error) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }
    if (cursor) {
        SDL_FreeCursor(cursor);
    }
    if (surface) {
        SDL_FreeSurface(surface);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return error;
#endif

	return 0;
}