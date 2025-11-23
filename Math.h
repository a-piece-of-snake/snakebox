#pragma once
#include <box2d/box2d.h>
#include <cmath>
#include <cstdint>
#include <vector>

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


inline int getGridPos(b2Vec2 pos, float cellSize, int gridSize)
{
    if (cellSize <= 0 || gridSize <= 0)
        return 0;

    int ix = static_cast<int>(std::floor(pos.x / cellSize));
    int iy = static_cast<int>(std::floor(pos.y / cellSize));

    uint32_t hash = static_cast<uint32_t>(ix) * 2654435761U ^ static_cast<uint32_t>(iy) * 2246822519U;

    return static_cast<int>(hash % static_cast<uint32_t>(gridSize));
}