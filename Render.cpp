#include "Render.h"

void drawB2(sf::RenderWindow& window, const std::vector<b2BodyId>& bodyIds)
{
    sf::VertexArray shapeVertexArray(sf::PrimitiveType::Triangles);
    sf::VertexArray outlineVertexArray(sf::PrimitiveType::Lines);
    for (const auto& bodyId : bodyIds)
    {
        int shapecount = b2Body_GetShapeCount(bodyId);
        b2ShapeId shapeIds[shapecount];
        b2Body_GetShapes(bodyId, shapeIds, shapecount);
        b2Vec2 pos = b2Body_GetPosition(bodyId);
        b2Rot rot = b2Body_GetRotation(bodyId);
        float angle = atan2(rot.s, rot.c);
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        for (const auto& shapeId : shapeIds)
        {
            b2ShapeType shapeType = b2Shape_GetType(shapeId);
            if (shapeType == b2_polygonShape) //这里可以用switch（应该把），只是我懒得做
            {
                b2Polygon shape = b2Shape_GetPolygon(shapeId);
                std::vector<b2Vec2> poses;

                std::vector<sf::Vertex> worldVertices;
                for (int i = 0; i < shape.count; i++)
                {
                    poses.push_back(shape.vertices[i]);
                    b2Vec2 localVertex = shape.vertices[i];
                    float rotatedX = localVertex.x * cosAngle - localVertex.y * sinAngle;
                    float rotatedY = localVertex.x * sinAngle + localVertex.y * cosAngle;
                    float screenX = pos.x + rotatedX;
                    float screenY = pos.y + rotatedY;
                    worldVertices.emplace_back(sf::Vector2f(screenX, screenY), Colors::b2BodyOutline);
                }

                for (int i = 0; i < shape.count; i++)
                {
                    outlineVertexArray.append(worldVertices[i]);
                    outlineVertexArray.append(worldVertices[(i + 1) % shape.count]);
                }

                auto triangles = triangulate(poses);
                for (const auto& triangle : triangles)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        int vertexIndex = triangle[i];
                        if (vertexIndex >= static_cast<int>(poses.size()))
                            continue;

                        b2Vec2 localVertex = poses[vertexIndex];

                        float rotatedX = localVertex.x * cosAngle - localVertex.y * sinAngle;
                        float rotatedY = localVertex.x * sinAngle + localVertex.y * cosAngle;

                        float screenX = pos.x + rotatedX;
                        float screenY = pos.y + rotatedY;

                        sf::Vertex vertex;
                        vertex.position = {screenX, screenY};
                        vertex.color = Colors::b2Body;
                        shapeVertexArray.append(vertex);
                    }
                }
            }
            else if (shapeType == b2_circleShape)
            {
                static const int EDGES = 16;
                b2Circle shape = b2Shape_GetCircle(shapeId);
                float r = shape.radius;


                b2Vec2 localCenter = shape.center;
                b2Vec2 worldCenter = {pos.x + localCenter.x * cosAngle - localCenter.y * sinAngle,
                                      pos.y + localCenter.x * sinAngle + localCenter.y * cosAngle};
                for (int i = 0; i < EDGES; ++i)
                {
                    float a1 = 2.0f * B2_PI * i / EDGES;
                    float a2 = 2.0f * B2_PI * (i + 1) / EDGES;

                    b2Vec2 v1 = {worldCenter.x + r * cos(a1), worldCenter.y + r * sin(a1)};
                    b2Vec2 v2 = {worldCenter.x + r * cos(a2), worldCenter.y + r * sin(a2)};
                    shapeVertexArray.append(sf::Vertex({worldCenter.x, worldCenter.y}, Colors::b2Body));
                    shapeVertexArray.append(sf::Vertex({v1.x, v1.y}, Colors::b2Body));
                    shapeVertexArray.append(sf::Vertex({v2.x, v2.y}, Colors::b2Body));
                    outlineVertexArray.append(sf::Vertex({v1.x, v1.y}, Colors::b2BodyOutline));
                    outlineVertexArray.append(sf::Vertex({v2.x, v2.y}, Colors::b2BodyOutline));
                }
                float indicatorLength = r * 0.8f;
                b2Vec2 tip = {worldCenter.x + indicatorLength * cos(angle),
                              worldCenter.y + indicatorLength * sin(angle)};
                outlineVertexArray.append(sf::Vertex({worldCenter.x, worldCenter.y}, Colors::b2BodyOutline));
                outlineVertexArray.append(sf::Vertex({tip.x, tip.y}, Colors::b2BodyOutline));
            }
            else
            {
                ERROR("Unknow box2d type!");
            }
        }
    }
    window.draw(shapeVertexArray);
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