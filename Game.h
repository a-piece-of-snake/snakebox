#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include <box2d/math_functions.h>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <thread>
#include <vector>

#include "ColorfulLog.h"
#include "Math.h"
#include "Render.h"
#include "Resources.h"
#include "Spawnable.h"
#include "ThreadPool.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"
class Game
{
public:
    Game();
    ~Game();

    void Init();
    void Run();
    void Destroy();

private:
    // 窗口和视图
    int width = 1920 / 1.25, height = 1080 / 1.25;
    sf::RenderWindow window;
    sf::View worldView;

    // 输入相关
    sf::Vector2f mousePos;
    sf::Vector2f lastmousePos;
    b2Vec2 worldPos = b2Vec2_zero;

    // 时间/FPS
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    int tickCount = 0;

    //资源
    TextureManager textureManager;
    SpawnableObjectBrowser spawnableBrowser;

    // 世界与对象
    bool pause = false;
    int particleCount = 0;
    std::vector<b2BodyId> bodyIds;
    b2WorldId worldId;
    ParticleWorld particleWorld;

    // 循环分解
    void InitWindow();
    void InitImGui();
    void InitFluid();
    void InitScene();
    void InitResources();
    void Update();
    void Render();
    void ImGuiRelated();
    void KeyLogic();
    void HandleEvent(const sf::Event& event);
    void UpdateWorld();
    void DrawWorld();
    void RenderUi();
};