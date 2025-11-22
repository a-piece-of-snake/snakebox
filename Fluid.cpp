#include "Fluid.h"

void ParticleGroup::createParticle(b2WorldId worldId, b2Vec2 pos)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;
    bodyDef.motionLocks.angularZ = true;
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Circle dynamicCircle;
    //INFO("%f", config.radius);
    dynamicCircle.radius = config.radius;
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
    b2CreateCircleShape(bodyId, &shapeDef, &dynamicCircle);
    Particle particle;
    particle.bodyId = bodyId;
    particle.group = this;
    particle.mass = b2Body_GetMass(bodyId);
    particle.pos = pos;
    particles.push_back(std::move(particle));
}

void ParticleWorld::init()
{
    for (ParticleGroup& group : groups)
    {
        group.particles.reserve(5000);
    }
}

void ParticleWorld::update()
{
    for (ParticleGroup& group : groups)
    {
        for (Particle& particle : group.particles)
        {
            particle.impulse = b2Vec2_zero;
            particle.pos = b2Body_GetPosition(particle.bodyId);
            particle.velocity = b2Body_GetLinearVelocity(particle.bodyId);
        }
    }
}