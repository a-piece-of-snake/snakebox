#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <box2d/box2d.h>
#include <cmath>

#include "ColorfulLog.h"
#include "Math.h"
#include "Resources.h"


namespace Colors
{
constexpr sf::Color b2Body = sf::Color{255, 255, 255, 127};
constexpr sf::Color b2BodyOutline = sf::Color{255, 255, 255, 255};
} // namespace Colors

void drawB2(sf::RenderWindow& window, std::vector<b2BodyId>& bodyIds);