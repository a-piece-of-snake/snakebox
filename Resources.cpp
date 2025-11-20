#include "Resources.h"

std::shared_ptr<sf::Texture> TextureManager::get(const std::string& name)
{
    auto it = cache.find(name);
    if (it != cache.end())
    {
        return it->second;
    }

    std::string path = "Assets/Textures/" + name;
    auto tex = std::make_shared<sf::Texture>();
    if (!tex->loadFromFile(path))
    {
        ERROR("Failed to load texture: {%s}", path.c_str());
        return nullptr;
    }

    cache[name] = tex;
    SUCCESS("Loaded texture: {%s}", path.c_str());
    return tex;
}

void TextureManager::clear()
{
    cache.clear();
}