#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include "res_path.h"
#include "cleanup.h"


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void logSDLError(std::ostream &os, const std::string &msg) {
  os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {

  SDL_Texture *texture = nullptr;

  SDL_Surface *loadedImage  = SDL_LoadBMP(file.c_str());
  if (loadedImage != nullptr){

    texture = SDL_CreateTextureFromSurface(ren, loadedImage);
    SDL_FreeSurface(loadedImage);
    if (texture == nullptr){
      logSDLError(std::cout, "CreateTextureFromSurface");
    }
  }
  else {
    logSDLError(std::cout, "LoadBMP");
  }

  return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;

  SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
  SDL_RenderCopy(ren, tex, NULL, &dst);
}

int main(int, char**){

  SDL_Rect ball;
  int iW, iH;
  int inc = 1;
  int inc2 = 1;
  SDL_Event e;
  bool quit = false;

  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    logSDLError(std::cout, "SDL_Init");
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (win == nullptr){
    logSDLError(std::cout, "CreateWindow");
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr){
    logSDLError(std::cout, "CreateRenderer");
    cleanup(win);
    SDL_Quit();
    return 1;
  }

  std::string imagePath = getResourcePath() + "ball.bmp";
  
  SDL_Texture *image = loadTexture(imagePath, ren);
  if (image == nullptr) {
    cleanup(image, ren, win);
    SDL_Quit();
    return 1;
  }


  SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
  
  // Initial position of the ball
  ball.w = iW/2;
  ball.h = iH/2;
  ball.x = random() % (SCREEN_WIDTH - ball.w);
  ball.y = random() % (SCREEN_HEIGHT - ball.h);

  std::cout << "Ball -- center (" << ball.x << ", " << ball.y << ") radius= " << ball.w << std::endl;


  while(!quit) {

    while (SDL_PollEvent(&e)){
      if (e.type == SDL_QUIT){
        quit = true;
      }
      if (e.type == SDL_KEYDOWN){
        quit = true;
      }
      if (e.type == SDL_MOUSEBUTTONDOWN){
        quit = true;
      }
    }

    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 0);

    renderTexture(image, ren, ball.x, ball.y);
    SDL_RenderPresent(ren);
    SDL_Delay(10);

    ball.x += (inc*10);
    ball.y += (inc2*10);
    if (ball.x + 2*ball.w >= SCREEN_WIDTH) {
      inc = -1;
    }
    else if (ball.x <= 0) {
      inc = 1;
    }

    if (ball.y + 2*ball.w >= SCREEN_HEIGHT) {
      inc2 = -1;
    }
    else if (ball.y <= 0) {
      inc2 = 1;
    }

  }

  cleanup(image, ren, win);
  SDL_Quit();

  return 0;
}
