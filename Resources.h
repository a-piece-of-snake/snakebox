#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <string>
#include <unordered_map>

#include "ColorfulLog.h"

class TextureManager
{
private:
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> cache;

public:
    std::shared_ptr<sf::Texture> get(const std::string& name);
    void clear();
};