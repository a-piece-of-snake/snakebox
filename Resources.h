#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <string>
#include <unordered_map>

#include "ColorfulLog.h"

class TextureManager
{
private:
    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> cache;
    std::shared_ptr<SDL_Texture> null;

public:
    void init(SDL_Renderer* renderer);
    std::shared_ptr<SDL_Texture> get(SDL_Renderer* renderer, const std::string& name);
    void clear();
};