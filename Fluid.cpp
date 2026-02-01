#include "Fluid.h"

void ParticleGroup::createParticle(b2WorldId worldId, b2Vec2 pos)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;
    // DEBUG("pos: %f, %f", bodyDef.position.x, bodyDef.position.y);
    bodyDef.fixedRotation = true;
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Circle dynamicCircle;
    dynamicCircle.radius = config.radius;
    dynamicCircle.center = b2Vec2_zero;
    // DEBUG("radius: %f", dynamicCircle.radius);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = config.density;
    shapeDef.material.friction = config.friction;
    shapeDef.material.restitution = config.restitution;
    {
        b2Filter filter;
        filter.categoryBits = 0x0002;
        filter.maskBits = 0xFFFF & ~(0x0002);
        shapeDef.filter = filter;
    }
    b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &dynamicCircle);
    // DEBUG("%f,%f,%f,%f",
    //      b2Shape_GetAABB(shapeId).lowerBound.x,
    //      b2Shape_GetAABB(shapeId).lowerBound.y,
    //      b2Shape_GetAABB(shapeId).upperBound.x,
    //      b2Shape_GetAABB(shapeId).upperBound.y);
    Particle particle;
    particle.bodyId = bodyId;
    particle.group = this;
    particle.mass = b2Body_GetMass(bodyId);
    particle.pos = pos;
    particles.push_back(std::move(particle));
}

void ParticleWorld::init()
{
    grids.clear();
    grids.resize(50000);

    for (auto& grid : grids)
    {
        grid.clear();
        grid.reserve(20);
    }
    for (ParticleGroup& group : groups)
    {
        group.particles.reserve(5000);
    }
}

void ParticleWorld::update()
{
    for (auto& grid : grids)
    {
        grid.clear();
    }
    for (ParticleGroup& group : groups)
    {
        for (int i = 0; i < group.particles.size(); ++i)
        {
            Particle& particle = group.particles[i];
            particle.index = i;
            particle.force = b2Vec2_zero;
            particle.pos = b2Body_GetPosition(particle.bodyId);
            particle.velocity = b2Body_GetLinearVelocity(particle.bodyId);
            grids[getGridIndex(particle.pos, group.config.radius * group.config.IMPACT, 50000)].push_back(&particle);
        }
    }

    for (auto& grid : grids)
    {
        if (!grid.empty())
        {
            std::sort(
                grid.begin(), grid.end(), [](const Particle* a, const Particle* b) { return a->index < b->index; });
        }
    }
}
void ParticleGroup::step(float timestep)
{
    float cellSize = config.radius * config.IMPACT;
    for (auto& p : particles)
    {
        auto gridPos = getGridPos(p.pos, cellSize);
        std::vector<Particle*> others;

        b2Vec2 argPos = b2Vec2_zero;
        float density = 0.f;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                SDL_Point otherGridPos = {gridPos.x + i, gridPos.y + j};
                for (auto& other : world->grids[getGridIndex(otherGridPos, 50000)])
                {
                    if (other->index != p.index)
                    {
                        if (b2Length(p.pos - other->pos) <= cellSize)
                        {
                            float force = getForce(b2Length(p.pos - other->pos), cellSize);
                            density += ((p.mass + other->mass) / 2) * force;
                            others.push_back(other);
                            argPos += other->pos;
                        }
                    }
                }
            }
        }
        for (auto& other : others)
        {
            b2Vec2 offect = p.pos - other->pos;
            b2Vec2 dir = b2Normalize(offect);
            float dist = b2Length(offect);
            b2Vec2 repulsionForce =
                (config.FORCE_MULTIPLIER) * dir * getForce(dist, cellSize) * (density * density * Q_sqrt(density));

            b2Vec2 totalForce = repulsionForce;

            other->force -= totalForce;
            p.force += totalForce;
        }
        if (others.size() != 0)
            p.force += {(argPos.x / others.size() - p.pos.x) * config.FORCE_SURFACE,
                        (argPos.y / others.size() - p.pos.y) * config.FORCE_SURFACE};
    }
    for (auto& p : particles)
    {
        // DEBUG("%f, %f", p.f  .x, p.impulse.y);
        b2Body_ApplyForceToCenter(p.bodyId, p.force, true);
    }
}
