#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <array>
#include <box2d/box2d.h>
#include <cmath>
#include <cstdint>
#include <vector>


inline SDL_FPoint operator+(const SDL_FPoint& a, const SDL_FPoint& b)
{
    return {a.x + b.x, a.y + b.y};
}

inline SDL_FPoint operator-(const SDL_FPoint& a, const SDL_FPoint& b)
{
    return {a.x - b.x, a.y - b.y};
}

inline SDL_FPoint& operator+=(SDL_FPoint& a, const SDL_FPoint& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline SDL_FPoint operator*(const SDL_FPoint& a, float scalar)
{
    return {a.x * scalar, a.y * scalar};
}


inline SDL_Point operator+(const SDL_Point& a, const SDL_Point& b)
{
    return {a.x + b.x, a.y + b.y};
}

inline SDL_Point operator-(const SDL_Point& a, const SDL_Point& b)
{
    return {a.x - b.x, a.y - b.y};
}

inline SDL_Point& operator+=(SDL_Point& a, const SDL_Point& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline SDL_Point operator*(const SDL_Point& a, int scalar)
{
    return {a.x * scalar, a.y * scalar};
}

inline float cross(const b2Vec2& a, const b2Vec2& b, const b2Vec2& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline bool pointInTriangle(const b2Vec2& a, const b2Vec2& b, const b2Vec2& c, const b2Vec2& p)
{
    float cross1 = cross(a, b, p);
    float cross2 = cross(b, c, p);
    float cross3 = cross(c, a, p);

    return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
}

inline bool triangleContainsPoint(
    const std::vector<b2Vec2>& vertices, int a, int b, int c, const std::vector<int>& indices)
{
    for (int index : indices)
    {
        if (index == a || index == b || index == c)
            continue;

        if (pointInTriangle(vertices[a], vertices[b], vertices[c], vertices[index]))
        {
            return true;
        }
    }
    return false;
}

inline bool isEar(const std::vector<b2Vec2>& vertices, int prev, int current, int next, const std::vector<int>& indices)
{
    if (cross(vertices[prev], vertices[current], vertices[next]) < 0)
    {
        return false;
    }

    return !triangleContainsPoint(vertices, prev, current, next, indices);
}

inline std::vector<std::vector<int>> triangulate(const std::vector<b2Vec2>& vertices)
{
    std::vector<std::vector<int>> triangles;
    if (vertices.size() < 3)
        return triangles;

    std::vector<int> indices;
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        indices.push_back(static_cast<int>(i));
    }

    int n = static_cast<int>(indices.size());
    int current = 0;

    while (n > 3)
    {
        int prev = (current - 1 + n) % n;
        int next = (current + 1) % n;

        if (isEar(vertices, indices[prev], indices[current], indices[next], indices))
        {
            std::vector<int> triangle = {indices[prev], indices[current], indices[next]};
            triangles.push_back(triangle);

            indices.erase(indices.begin() + current);
            n--;

            current = std::max(0, current - 1);
        }
        else
        {
            current = (current + 1) % n;
        }
    }

    if (n == 3)
    {
        std::vector<int> triangle = {indices[0], indices[1], indices[2]};
        triangles.push_back(triangle);
    }

    return triangles;
}


inline SDL_Point getGridPos(const b2Vec2& pos, const float& cellSize)
{
    return {static_cast<int>(std::floor(pos.x / cellSize)), static_cast<int>(std::floor(pos.y / cellSize))};
}

inline int getGridIndex(const b2Vec2& pos, const float& cellSize, const int& gridSize)
{
    if (cellSize <= 0 || gridSize <= 0)
        return 0;
    auto gridPos = getGridPos(pos, cellSize);
    int ix = gridPos.x;
    int iy = gridPos.y;

    uint32_t hash = static_cast<uint32_t>(ix) * 2654435761U ^ static_cast<uint32_t>(iy) * 2246822519U;

    return static_cast<int>(hash % static_cast<uint32_t>(gridSize));
}

inline int getGridIndex(const SDL_Point& gridPos, const int& gridSize)
{
    int ix = gridPos.x;
    int iy = gridPos.y;
    uint32_t hash = static_cast<uint32_t>(ix) * 2654435761U ^ static_cast<uint32_t>(iy) * 2246822519U;

    return static_cast<int>(hash % static_cast<uint32_t>(gridSize));
}


inline float Q_rsqrt(const float& number)
{ //Quake III Arena
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;            // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1); // what the f***?
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y));
    return y;
}


inline float Q_sqrt(const float& x)
{
    return x * Q_rsqrt(x);
}

inline float getForce(const float& dist, const float& radius)
{
    if (dist >= radius)
        return 0;
    float x = radius - dist;
    float radius_sq = radius * radius;
    return x * x * Q_sqrt(x) / (B2_PI / 4.0f * radius_sq * radius_sq);
}

inline SDL_FPoint normalize(const SDL_FPoint& a)
{
    float length = Q_sqrt(a.x * a.x + a.y * a.y);
    if (length > 0.f)
    {
        return {a.x / length, a.y / length};
    }
    return {0, 0};
}

inline SDL_FPoint getNormal(const SDL_FPoint& a, const SDL_FPoint& b)
{
    SDL_FPoint dir = normalize(b - a);
    SDL_FPoint normal = {-dir.y, dir.x};
    return normal;
}

inline std::array<SDL_FPoint, 4> getLine(const SDL_FPoint& a, const SDL_FPoint& b, float width)
{
    SDL_FPoint offset = getNormal(a, b) * (width * 0.5f);
    return {a + offset, a - offset, b + offset, b - offset};
}

inline float HueToRGB(float v1, float v2, float vH)
{
    if (vH < 0.0f)
        vH += 1.0f;
    if (vH > 1.0f)
        vH -= 1.0f;
    if ((6.0f * vH) < 1.0f)
        return (v1 + (v2 - v1) * 6.0f * vH);
    if ((2.0f * vH) < 1.0f)
        return (v2);
    if ((3.0f * vH) < 2.0f)
        return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f);
    return v1;
}

inline SDL_Color HSLAtoRGBA(float h, float s, float l, float a = 1.0f)
{
    float r, g, b;
    h /= 360.0f;

    if (s == 0)
    {
        r = g = b = l;
    }
    else
    {
        float v2 = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - (l * s));
        float v1 = 2.0f * l - v2;

        r = HueToRGB(v1, v2, h + (1.0f / 3.0f));
        g = HueToRGB(v1, v2, h);
        b = HueToRGB(v1, v2, h - (1.0f / 3.0f));
    }

    return {static_cast<Uint8>(r * 255),
            static_cast<Uint8>(g * 255),
            static_cast<Uint8>(b * 255),
            static_cast<Uint8>(a * 255)};
}
inline SDL_FColor HSLAtoRGBA_F(float h, float s, float l, float a = 1.0f)
{
    float r, g, b;
    h /= 360.0f;

    if (s == 0)
    {
        r = g = b = l;
    }
    else
    {
        float v2 = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - (l * s));
        float v1 = 2.0f * l - v2;

        r = HueToRGB(v1, v2, h + (1.0f / 3.0f));
        g = HueToRGB(v1, v2, h);
        b = HueToRGB(v1, v2, h - (1.0f / 3.0f));
    }
    return {r, g, b, a};
}
