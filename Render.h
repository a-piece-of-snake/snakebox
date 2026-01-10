#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <box2d/box2d.h>
#include <cmath>

#include "ColorfulLog.h"
#include "Fluid.h"
#include "Math.h"
#include "Resources.h"

namespace Colors
{
constexpr sf::Color b2Body = sf::Color{255, 255, 255, 127};
constexpr sf::Color b2BodyOutline = sf::Color{255, 255, 255, 255};
} // namespace Colors

void drawB2(sf::RenderWindow& window, const std::vector<b2BodyId>& bodyIds, sf::Texture* texture = nullptr);
void drawFluid(sf::RenderWindow& window, const ParticleWorld& particleWorld);