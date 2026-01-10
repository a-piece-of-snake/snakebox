#include "Resources.h"

void TextureManager::init()
{
    sf::Image image({2, 2}, sf::Color::Black);
    image.setPixel({0, 0}, sf::Color::Magenta);
    image.setPixel({1, 1}, sf::Color::Magenta);
    image.setPixel({0, 1}, sf::Color::Black);
    image.setPixel({1, 0}, sf::Color::Black);
    auto tex = std::make_shared<sf::Texture>();
    if (!tex->loadFromImage(image))
        ERROR("Failed to init texture manager!");
    null = tex;
}

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
        return null;
    }

    cache[name] = tex;
    SUCCESS("Loaded texture: {%s}", path.c_str());
    return tex;
}

void TextureManager::clear()
{
    cache.clear();
}

std::shared_ptr<sf::Shader> ShaderManager::get(const std::string& name)
{
    auto it = cache.find(name);
    if (it != cache.end())
    {
        return it->second;
    }

    std::string path = "Assets/Shaders/" + name;
    std::string fragPath = path + "Frag.glsl";
    std::string geoPath = path + "Geo.glsl";
    std::string vertPath = path + "Vert.glsl";
    auto shader = std::make_shared<sf::Shader>();
    if (std::filesystem::exists(fragPath))
    {
        if (!shader->loadFromFile(fragPath, sf::Shader::Type::Fragment))
        {
            ERROR("Failed to load fragment shader : {%s}", fragPath.c_str());
        }
    }
    else
    {
        WARN("Fragment shader: {%s} not found", fragPath.c_str());
    }
    if (std::filesystem::exists(geoPath))
    {
        if (!shader->loadFromFile(geoPath, sf::Shader::Type::Geometry))
        {
            ERROR("Failed to load geometry shader : {%s}", geoPath.c_str());
        }
    }
    else
    {
        WARN("Geometry shader: {%s} not found", geoPath.c_str());
    }
    if (std::filesystem::exists(vertPath))
    {
        if (!shader->loadFromFile(vertPath, sf::Shader::Type::Vertex))
        {
            ERROR("Failed to load vertex shader : {%s}", vertPath.c_str());
        }
    }
    else
    {
        WARN("Vertex shader: {%s} not found", vertPath.c_str());
    }

    cache[name] = shader;
    SUCCESS("Loaded shader: {%s}", name.c_str());
    return shader;
}

void ShaderManager::clear()
{
    cache.clear();
}