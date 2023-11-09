// project referenced nuget SDL2-CS wrapper package for language bindings:
// https://github.com/flibitijibibo/SDL2-CS
// https://www.libsdl.org/languages.php

// Simple DirectMedia Layer is a cross-platform development library for
//   low-level access HCI devices, OpenGL, Direct3D and etc GUI framework.
//   For example, GUI of ffplay(ffmpeg)

// this project should copy related sdl2 native dll files from nuget sdl2 redist packages
//   located <user>/.nuget/packages/sdl2xxxx/<version>/build/native
// tarent: it better download release binaries from github page, for the dependencies of these dependencies
// The wrapper provides bindings for the following libraries:
// - SDL2
// - SDL2_gfx
// - SDL2_image
// - SDL2_mixer
// - SDL2_ttf

using SDL2;

SDL.SDL_SetHint(SDL.SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

SDL.SDL_Init(SDL.SDL_INIT_VIDEO);

IntPtr window;
IntPtr renderer;
var err = SDL.SDL_CreateWindowAndRenderer(
    300, 200, SDL.SDL_WindowFlags.SDL_WINDOW_RESIZABLE, out window, out renderer);
if (err != 0)
{
    Console.WriteLine($"create window failed, err:{SDL.SDL_GetError()}");
}

// ensure the working directory, properties -> debug -> launch profile
IntPtr surface = SDL_image.IMG_Load("../Lenna_test_image.png");
if (surface == IntPtr.Zero)
{
    Console.WriteLine("image load failed!");
}

IntPtr texture = SDL.SDL_CreateTextureFromSurface(renderer, surface);
if (texture == IntPtr.Zero)
{
    Console.WriteLine("texture create failed");
}

// get texture original size
uint format;
int access;
int width;
int height;
err = SDL.SDL_QueryTexture(texture, out format, out access, out width, out height);
if (err != 0)
{
    Console.Write($"query texture failed, err={SDL.SDL_GetError()}");
}

SDL.SDL_SetWindowSize(window, width, height);

SDL.SDL_FreeSurface(surface);

SDL.SDL_Event evt;
while (true)
{
    SDL.SDL_PollEvent(out evt);
    if (evt.type == SDL.SDL_EventType.SDL_QUIT)
        break;

    SDL.SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL.SDL_RenderClear(renderer);
    SDL.SDL_RenderCopy(renderer, texture, IntPtr.Zero, IntPtr.Zero);
    SDL.SDL_RenderPresent(renderer);
}

SDL.SDL_DestroyTexture(texture);
SDL.SDL_DestroyRenderer(renderer);
SDL.SDL_DestroyWindow(window);

SDL.SDL_Quit();

// See https://aka.ms/new-console-template for more information
Console.WriteLine("Hello, World!");


