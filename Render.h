#pragma once

#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include <cmath>

#include "ColorfulLog.h"
#include "Fluid.h"
#include "Math.h"
#include "Resources.h"

namespace Colors
{
constexpr SDL_FColor b2Body = SDL_FColor{0.55f, 0.66f, 0.64f, 0.5f};
constexpr SDL_FColor b2BodyOutline = SDL_FColor{0.82f, 0.53f, 0.64f, 1.0f};
constexpr SDL_FColor b2Joint = SDL_FColor{0.69f, 0.55f, 0.70f, 1.0f};
} // namespace Colors

void drawB2(SDL_Renderer* renderer, const std::vector<b2BodyId>& bodyIds, SDL_Texture* texture = nullptr);
void drawFluid(SDL_Renderer* renderer, const ParticleWorld& particleWorld);
