#include "Render.h"
void drawB2(sf::RenderWindow& window, const std::vector<b2BodyId>& bodyIds, sf::Texture* texture)
{
    sf::VertexArray shapeVertexArray(sf::PrimitiveType::Triangles);
    sf::VertexArray outlineVertexArray(sf::PrimitiveType::Lines);

    for (const auto& bodyId : bodyIds)
    {
        int shapecount = b2Body_GetShapeCount(bodyId);
        std::vector<b2ShapeId> shapeIds(shapecount);
        b2Body_GetShapes(bodyId, shapeIds.data(), shapecount);
        b2Vec2 pos = b2Body_GetPosition(bodyId);
        b2Rot rot = b2Body_GetRotation(bodyId);
        float angle = atan2(rot.s, rot.c);
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

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

                std::vector<sf::Vertex> worldVertices;
                for (int i = 0; i < shape.count; ++i)
                {
                    b2Vec2 local = shape.vertices[i];

                    float wx = pos.x + local.x * cosAngle - local.y * sinAngle;
                    float wy = pos.y + local.x * sinAngle + local.y * cosAngle;

                    sf::Vertex v;
                    v.position = sf::Vector2f(wx, wy);

                    if (texture)
                    {
                        float u = (local.x - minX) / width;
                        float v_uv = (local.y - minY) / height;
                        v.texCoords = sf::Vector2f(u * texture->getSize().x, v_uv * texture->getSize().y);
                    }
                    else
                    {
                        v.color = Colors::b2Body;
                    }
                    worldVertices.push_back(v);
                }

                for (int i = 0; i < shape.count; ++i)
                {
                    sf::Vertex v1 = worldVertices[i];
                    sf::Vertex v2 = worldVertices[(i + 1) % shape.count];
                    if (!texture)
                    {
                        v1.color = v2.color = Colors::b2BodyOutline;
                    }
                    outlineVertexArray.append(v1);
                    outlineVertexArray.append(v2);
                }

                auto triangles = triangulate(std::vector<b2Vec2>(shape.vertices, shape.vertices + shape.count));
                for (const auto& tri : triangles)
                {
                    for (int idx : tri)
                    {
                        if (idx >= 0 && idx < static_cast<int>(worldVertices.size()))
                        {
                            shapeVertexArray.append(worldVertices[idx]);
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

                    sf::Vertex centerV, v1, v2;
                    centerV.position = sf::Vector2f(cx, cy);
                    v1.position = sf::Vector2f(w1.x, w1.y);
                    v2.position = sf::Vector2f(w2.x, w2.y);

                    if (texture)
                    {
                        float u_center = (center.x - minX) / width;
                        float v_center = (center.y - minY) / height;
                        centerV.texCoords =
                            sf::Vector2f(u_center * texture->getSize().x, v_center * texture->getSize().y);

                        float u1 = (local1.x - minX) / width;
                        float v1_uv = (local1.y - minY) / height;
                        float u2 = (local2.x - minX) / width;
                        float v2_uv = (local2.y - minY) / height;

                        v1.texCoords = sf::Vector2f(u1 * texture->getSize().x, v1_uv * texture->getSize().y);
                        v2.texCoords = sf::Vector2f(u2 * texture->getSize().x, v2_uv * texture->getSize().y);
                    }
                    else
                    {
                        centerV.color = v1.color = v2.color = Colors::b2Body;
                    }

                    shapeVertexArray.append(centerV);
                    shapeVertexArray.append(v1);
                    shapeVertexArray.append(v2);

                    if (!texture)
                    {
                        v1.color = v2.color = Colors::b2BodyOutline;
                    }
                    outlineVertexArray.append(v1);
                    outlineVertexArray.append(v2);
                }

                float indicatorLength = r * 0.8f;
                b2Vec2 tipLocal = {center.x + indicatorLength * cos(angle), center.y + indicatorLength * sin(angle)};
                b2Vec2 tipWorld = {pos.x + tipLocal.x * cosAngle - tipLocal.y * sinAngle,
                                   pos.y + tipLocal.x * sinAngle + tipLocal.y * cosAngle};
                outlineVertexArray.append(sf::Vertex(sf::Vector2f(cx, cy), Colors::b2BodyOutline));
                outlineVertexArray.append(sf::Vertex(sf::Vector2f(tipWorld.x, tipWorld.y), Colors::b2BodyOutline));
            }
        }
    }

    if (texture)
    {
        sf::RenderStates states;
        states.texture = texture;
        window.draw(shapeVertexArray, states);
    }
    else
    {
        window.draw(shapeVertexArray);
    }
    window.draw(outlineVertexArray);
}
void drawFluid(sf::RenderWindow& window, const ParticleWorld& particleWorld)
{
    static const b2Vec2 offect[3] = {{1, 0}, {-1, 1}, {-1, -1}};
    sf::VertexArray shapeVertexArray(sf::PrimitiveType::Triangles);
    sf::VertexArray outlineVertexArray(sf::PrimitiveType::Lines);
    for (const ParticleGroup& group : particleWorld.groups)
    {
        float radius = group.config.radius;
        for (const Particle& particle : group.particles)
        {
            b2Vec2 pos1 = particle.pos + offect[0] * radius;
            b2Vec2 pos2 = particle.pos + offect[1] * radius;
            b2Vec2 pos3 = particle.pos + offect[2] * radius;
            shapeVertexArray.append(sf::Vertex({pos1.x, pos1.y}, Colors::b2Body));
            shapeVertexArray.append(sf::Vertex({pos2.x, pos2.y}, Colors::b2Body));
            shapeVertexArray.append(sf::Vertex({pos3.x, pos3.y}, Colors::b2Body));
            outlineVertexArray.append(sf::Vertex({pos1.x, pos1.y}, Colors::b2BodyOutline));
            outlineVertexArray.append(sf::Vertex({pos2.x, pos2.y}, Colors::b2BodyOutline));
            outlineVertexArray.append(sf::Vertex({pos2.x, pos2.y}, Colors::b2BodyOutline));
            outlineVertexArray.append(sf::Vertex({pos3.x, pos3.y}, Colors::b2BodyOutline));
            outlineVertexArray.append(sf::Vertex({pos3.x, pos3.y}, Colors::b2BodyOutline));
            outlineVertexArray.append(sf::Vertex({pos1.x, pos1.y}, Colors::b2BodyOutline));
        }
    }
    window.draw(shapeVertexArray);
    window.draw(outlineVertexArray);
}