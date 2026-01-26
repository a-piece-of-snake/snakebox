#include "Render.h"

#include <algorithm>

#include "Math.h"

const float WIDTH = 1;
static std::vector<SDL_Vertex> shapeVertices;
static std::vector<SDL_Vertex> outlineVertices;

std::vector<SDL_FColor>& getColorStyle()
{
    static std::vector<SDL_FColor> colorStyle = {SDL_FColor{0.55f, 0.66f, 0.64f, 0.5f},
                                                 SDL_FColor{0.82f, 0.53f, 0.64f, 1.0f},
                                                 SDL_FColor{0.69f, 0.55f, 0.70f, 1.0f}};
    return colorStyle;
}

void makeLine(
    std::vector<SDL_Vertex>* vertexs, const SDL_FPoint& a, const SDL_FPoint& b, float width, const SDL_FColor& fcolor)
{
    auto [p1, p2, p3, p4] = getLine(a, b, width);

    vertexs->emplace_back(SDL_Vertex{p1, fcolor});
    vertexs->emplace_back(SDL_Vertex{p2, fcolor});
    vertexs->emplace_back(SDL_Vertex{p3, fcolor});

    vertexs->emplace_back(SDL_Vertex{p2, fcolor});
    vertexs->emplace_back(SDL_Vertex{p3, fcolor});
    vertexs->emplace_back(SDL_Vertex{p4, fcolor});
}
void drawB2(SDL_Renderer* renderer, const std::vector<b2BodyId>& bodyIds, SDL_Texture* texture)
{
    shapeVertices.clear();
    outlineVertices.clear();
    float hue = fmodf(SDL_GetTicks() / 10.0f, 360.0f);
    SDL_FColor outlineColor = HSLAtoRGBA_F(hue, 1.f, 0.5f);
    for (const auto& bodyId : bodyIds)
    {
        int shapecount = b2Body_GetShapeCount(bodyId);
        std::vector<b2ShapeId> shapeIds(shapecount);
        b2Body_GetShapes(bodyId, shapeIds.data(), shapecount);
        b2Vec2 pos = b2Body_GetPosition(bodyId);
        b2Rot rot = b2Body_GetRotation(bodyId);
        float cosAngle = rot.c;
        float sinAngle = rot.s;
        // float angle = atan2(rot.s, rot.c);
        // float cosAngle = cos(angle);
        // float sinAngle = sin(angle);
        for (const auto& shapeId : shapeIds)
        {
            b2ShapeType shapeType = b2Shape_GetType(shapeId);
            if (shapeType == b2_polygonShape)
            {
                b2Polygon shape = b2Shape_GetPolygon(shapeId);

                float minX = FLT_MAX, maxX = -FLT_MAX;
                float minY = FLT_MAX, maxY = -FLT_MAX;
                for (int i = 0; i < shape.count; ++i)
                {
                    b2Vec2 v = shape.vertices[i];
                    minX = std::min(minX, v.x);
                    maxX = std::max(maxX, v.x);
                    minY = std::min(minY, v.y);
                    maxY = std::max(maxY, v.y);
                }
                float width = std::max(maxX - minX, 1e-5f);
                float height = std::max(maxY - minY, 1e-5f);

                std::vector<SDL_Vertex> worldVertices;
                for (int i = 0; i < shape.count; ++i)
                {
                    b2Vec2 local = shape.vertices[i];

                    float wx = pos.x + local.x * cosAngle - local.y * sinAngle;
                    float wy = pos.y + local.x * sinAngle + local.y * cosAngle;

                    SDL_Vertex v;
                    v.position = SDL_FPoint{wx, wy};

                    if (texture)
                    {
                        float u = (local.x - minX) / width;
                        float v_uv = (local.y - minY) / height;
                        v.tex_coord = SDL_FPoint{u * texture->h, v_uv * texture->w};
                    }
                    else
                    {
                        v.color = getColorStyle()[ColorKeys::b2Body];
                    }
                    worldVertices.push_back(v);
                }

                for (int i = 0; i < shape.count; ++i)
                {
                    makeLine(&outlineVertices,
                             worldVertices[i].position,
                             worldVertices[(i + 1) % shape.count].position,
                             WIDTH,
                             outlineColor);
                }

                auto triangles = triangulate(std::vector<b2Vec2>(shape.vertices, shape.vertices + shape.count));
                for (const auto& tri : triangles)
                {
                    for (int idx : tri)
                    {
                        if (idx >= 0 && idx < static_cast<int>(worldVertices.size()))
                        {
                            shapeVertices.push_back(worldVertices[idx]);
                        }
                    }
                }
            }
            else if (shapeType == b2_circleShape)
            {
                static const int EDGES = 16;
                b2Circle shape = b2Shape_GetCircle(shapeId);
                b2Vec2 center = shape.center;
                float r = shape.radius;

                float minX = center.x - r, maxX = center.x + r;
                float minY = center.y - r, maxY = center.y + r;
                float width = 2 * r, height = 2 * r;

                float cx = pos.x + center.x * cosAngle - center.y * sinAngle;
                float cy = pos.y + center.x * sinAngle + center.y * cosAngle;

                for (int i = 0; i < EDGES; ++i)
                {
                    float a1 = 2.0f * B2_PI * i / EDGES;
                    float a2 = 2.0f * B2_PI * (i + 1) / EDGES;

                    b2Vec2 local1 = {center.x + r * cos(a1), center.y + r * sin(a1)};
                    b2Vec2 local2 = {center.x + r * cos(a2), center.y + r * sin(a2)};

                    b2Vec2 w1 = {pos.x + local1.x * cosAngle - local1.y * sinAngle,
                                 pos.y + local1.x * sinAngle + local1.y * cosAngle};
                    b2Vec2 w2 = {pos.x + local2.x * cosAngle - local2.y * sinAngle,
                                 pos.y + local2.x * sinAngle + local2.y * cosAngle};

                    SDL_Vertex centerV, v1, v2;
                    centerV.position = SDL_FPoint{cx, cy};
                    v1.position = SDL_FPoint{w1.x, w1.y};
                    v2.position = SDL_FPoint{w2.x, w2.y};

                    if (texture)
                    {
                        float u_center = (center.x - minX) / width;
                        float v_center = (center.y - minY) / height;
                        centerV.tex_coord = SDL_FPoint{u_center * texture->h, v_center * texture->w};

                        float u1 = (local1.x - minX) / width;
                        float v1_uv = (local1.y - minY) / height;
                        float u2 = (local2.x - minX) / width;
                        float v2_uv = (local2.y - minY) / height;

                        v1.tex_coord = SDL_FPoint{u1 * texture->h, v1_uv * texture->w};
                        v2.tex_coord = SDL_FPoint{u2 * texture->h, v2_uv * texture->w};
                    }
                    else
                    {
                        centerV.color = v1.color = v2.color = getColorStyle()[ColorKeys::b2Body];
                    }

                    shapeVertices.push_back(centerV);
                    shapeVertices.push_back(v1);
                    shapeVertices.push_back(v2);
                    makeLine(&outlineVertices, v1.position, v2.position, WIDTH, outlineColor);
                }

                float indicatorLength = r * 0.8f;
                b2Vec2 tipLocal = {center.x + indicatorLength * cosAngle, center.y + indicatorLength * sinAngle};
                b2Vec2 tipWorld = {pos.x + tipLocal.x * cosAngle - tipLocal.y * sinAngle,
                                   pos.y + tipLocal.x * sinAngle + tipLocal.y * cosAngle};
                makeLine(&outlineVertices, {cx, cy}, {tipWorld.x, tipWorld.y}, WIDTH, outlineColor);
            }
        }
    }

    SDL_RenderGeometry(renderer, texture, shapeVertices.data(), shapeVertices.size(), nullptr, 0);
    SDL_RenderGeometry(renderer, nullptr, outlineVertices.data(), outlineVertices.size(), nullptr, 0);
    return;
}
void drawFluid(SDL_Renderer* renderer, const ParticleWorld& particleWorld)
{
    size_t totalParticles = 0;
    for (const auto& g : particleWorld.groups)
        totalParticles += g.particles.size();
    shapeVertices.clear();
    shapeVertices.reserve(totalParticles * 3);
    outlineVertices.clear();
    outlineVertices.reserve(totalParticles * 18);
    float hue = fmodf(SDL_GetTicks() / 10.0f, 360.0f);
    SDL_FColor outlineColor = HSLAtoRGBA_F(hue, 1.f, 0.5f);
    static const b2Vec2 offect[3] = {{1, 0}, {-1, 1}, {-1, -1}};
    for (const ParticleGroup& group : particleWorld.groups)
    {
        float radius = group.config.radius;
        for (const Particle& particle : group.particles)
        {
            b2Vec2 pos1 = particle.pos + offect[0] * radius;
            b2Vec2 pos2 = particle.pos + offect[1] * radius;
            b2Vec2 pos3 = particle.pos + offect[2] * radius;
            shapeVertices.emplace_back(
                SDL_FPoint{pos1.x, pos1.y}, getColorStyle()[ColorKeys::b2Body], SDL_FPoint{0, 0});
            shapeVertices.emplace_back(
                SDL_FPoint{pos2.x, pos2.y}, getColorStyle()[ColorKeys::b2Body], SDL_FPoint{0, 0});
            shapeVertices.emplace_back(
                SDL_FPoint{pos3.x, pos3.y}, getColorStyle()[ColorKeys::b2Body], SDL_FPoint{0, 0});
            makeLine(&outlineVertices, {pos1.x, pos1.y}, {pos2.x, pos2.y}, WIDTH, outlineColor);
            makeLine(&outlineVertices, {pos2.x, pos2.y}, {pos3.x, pos3.y}, WIDTH, outlineColor);
            makeLine(&outlineVertices, {pos3.x, pos3.y}, {pos1.x, pos1.y}, WIDTH, outlineColor);
        }
    }
    SDL_RenderGeometry(renderer, nullptr, shapeVertices.data(), shapeVertices.size(), nullptr, 0);
    SDL_RenderGeometry(renderer, nullptr, outlineVertices.data(), outlineVertices.size(), nullptr, 0);
    return;
}
