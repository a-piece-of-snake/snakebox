#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <functional>
#include <vector>

#include "ColorfulLog.h"
#include "ThreadPool.h"


class ParticleWorld;
class ParticleGroup;

struct ParticleConfig
{
    float density = 1.f;
    float radius = 4.f;
    float friction = 0.f;
    float restitution = 0.01f;
    float IMPACT = 5.f;
    float MOMENTUM_COEFFICIENT = 1.f;
    float FORCE_MULTIPLIER = 40000.f;
    float FORCE_SURFACE = 50.f;
    float FORCE_ADHESION = 0.f;
    float SHEAR_VISCOSITY = 20.f;
    float VISCOSITY = 0.f;
    float VISCOSITY_LEAVE = 0.f;
    float MAX_GET_FORCE = 1250.f;
    float MAX_FORCE = 175000.f;
    float MIN_DENSITY = 0.25f;
    float MAX_DENSITY = 3.f;
    std::string NAME = "Water";
};


struct Particle
{
    int index;
    b2Vec2 pos = b2Vec2_zero;
    b2Vec2 velocity = b2Vec2_zero;
    b2Vec2 impulse = b2Vec2_zero;
    float mass = 1.f;
    b2BodyId bodyId;
    ParticleGroup* group;
};

class ParticleGroup
{
public:
    ParticleWorld* world;
    ParticleConfig config;
    sf::Color color = sf::Color::White;
    std::vector<Particle> particles;
    /*
    std::vector<b2Vec2> p_pos;
    std::vector<b2Vec2> p_velocity;
    std::vector<b2Vec2> p_impulse;
    std::vector<b2BodyId> p_bodyId;
    std::vector<float> p_mass;*/

public:
    void createParticle(b2WorldId worldId, b2Vec2 pos);
    void destoryParticle();
    void step(float timestep);
};

class ParticleWorld
{
private:
    ThreadPool threadPool{4};
    std::vector<std::vector<Particle*>> grids;

public:
    std::vector<ParticleGroup> groups;
    void init();
    void update();
    void step(float timestep);
};