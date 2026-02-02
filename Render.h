#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <cmath>
#include <cstddef>

#include "ColorfulLog.h"
#include "Fluid.h"
#include "Math.h"
#include "Resources.h"

namespace ColorKeys
{
enum colorKeys : size_t
{
    b2Body = 0,
    b2BodyOutline,
    b2Joint,
    COUNT
};
} // namespace ColorKeys

class Camera
{
public:
    float x, y;
    float rot;
    float zoom;

private:
};

std::vector<SDL_FColor>& getColorStyle();

void makeLine(
    std::vector<SDL_Vertex>* vertexs, const SDL_FPoint& a, const SDL_FPoint& b, float width, const SDL_FColor& fcolor);

void drawB2(SDL_Renderer* renderer, const std::vector<b2BodyId>& bodyIds, SDL_Texture* texture = nullptr);
void drawFluid(SDL_Renderer* renderer, const ParticleWorld& particleWorld);
