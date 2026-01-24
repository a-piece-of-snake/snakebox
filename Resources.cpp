#include "Resources.h"

void TextureManager::init(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_RGBA8888);
    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    int pitch_pixels = surface->pitch / 4;
    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(surface->format);
    uint32_t magenta = SDL_MapRGBA(details, NULL, 255, 0, 255, 255);
    uint32_t black = SDL_MapRGBA(details, NULL, 0, 0, 0, 255);

    pixels[0 * pitch_pixels + 0] = magenta; // (0, 0)
    pixels[1 * pitch_pixels + 1] = magenta; // (1, 1)
    pixels[1 * pitch_pixels + 0] = black;   // (0, 1)
    pixels[0 * pitch_pixels + 1] = black;   // (1, 0)

    SDL_UnlockSurface(surface);
    SDL_Texture* rawTex = SDL_CreateTextureFromSurface(renderer, surface);

    if (!rawTex)
    {
        ERROR("Failed to create texture: %s", SDL_GetError());
    }

    null = std::shared_ptr<SDL_Texture>(rawTex, SDL_DestroyTexture);
    SDL_DestroySurface(surface);
}

std::shared_ptr<SDL_Texture> TextureManager::get(SDL_Renderer* renderer, const std::string& name)
{
    auto it = cache.find(name);
    if (it != cache.end())
    {
        return it->second;
    }

    std::string path = "Assets/Textures/" + name;
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface)
    {
        ERROR("Failed to load texture surface: %s", path.c_str());
        return null;
    }

    SDL_Texture* rawTex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!rawTex)
    {
        ERROR("Failed to create texture: %s", SDL_GetError());
    }
    std::shared_ptr<SDL_Texture> tex(rawTex, SDL_DestroyTexture);
    SDL_DestroySurface(surface);
    cache[name] = tex;
    SUCCESS("Loaded texture: {%s}", path.c_str());
    return tex;
}

void TextureManager::clear()
{
    cache.clear();
}
