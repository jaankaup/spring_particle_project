#include <stdexcept>
//#include <SDL2/SDL.h>
#include "window.h"
#include "../Utils/log.h"

Window& Window::getInstance()
{
    static Window instance;
    return instance;
}

Window::Window()
{
    init(1200,800);
}

Window::~Window()
{
  dispose();
}

void Window::dispose()
{
  if (pContext != NULL) { SDL_GL_DeleteContext(pContext); }
  if (pWindow != NULL) { SDL_DestroyWindow(pWindow); }
  SDL_Quit(); 
}

bool Window::init(int width, int height)
{
//  Log::getDebug().log("%","Window::Window. Creating window.");
  SDL_Init(SDL_INIT_VIDEO);
//  Log::getDebug().log("%",std::to_string(sdl));
  if (!SDL_WasInit(SDL_INIT_VIDEO))
  {
    //Log::getDebug().log("%","Window::Window. initializing SDL.");
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      Log::getError().log("%","Window::Window. Failed to initialize SDL.");
      return false;
    }
  }
  Log::getDebug().log("%","Window::Window. window created.");
    int x = SDL_WINDOWPOS_UNDEFINED;
    int y = SDL_WINDOWPOS_UNDEFINED;
    int flags = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
//    int flags = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
    pWindow = SDL_CreateWindow("", x, y, width, height, flags);

    if (pWindow == NULL)
    {
      std::string error = SDL_GetError();
      Log::getError().log("%.%","Window::init. Failed to create SDL_Window",error);
      return false;
    }


    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    #ifdef EMSCRIPTEN
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif

    #ifndef EMSCRIPTEN
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    #endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    /* Luodaan opengl conteksti ikkunalle. */
    pContext = SDL_GL_CreateContext(pWindow);
    if (pContext == NULL)
    {
        std::string error = SDL_GetError();
        Log::getError().log("%.%","Window::init. Failed to create SDL_GLContext.",error);
        Log::getError().log("Perhaps your hardware doesn't support opengl 4.3.");
    }

    glewExperimental = GL_TRUE;


    GLenum error = glewInit();
    if (error != GLEW_NO_ERROR) {
        Log::getError().log("%","Window::init. Failed to initialize glew.");
        return false;
     }

    Log::getInfo().log("GL_Version: %", glGetString(GL_VERSION));
    Log::getInfo().log("Vendor: %", glGetString(GL_VENDOR));
    Log::getInfo().log("Renderer: %", glGetString(GL_RENDERER));
    SDL_GL_SetSwapInterval(true);

    return true;
}

void Window::swapBuffers()
{
  SDL_GL_SwapWindow(pWindow);
}

void Window::resize(int w, int h)
{
    SDL_SetWindowSize(pWindow, w, h);
    glViewport(0, 0, w, h);

    SDL_Event e;
    e.type = SDL_WINDOWEVENT;
    e.window.event = SDL_WINDOWEVENT_RESIZED;
    e.window.windowID = SDL_GetWindowID(pWindow);
    e.window.data1 = w;
    e.window.data2 = h;
    SDL_PushEvent(&e);
}
