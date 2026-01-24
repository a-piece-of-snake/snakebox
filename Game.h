#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <box2d/box2d.h>
#include <box2d/collision.h>
#include <box2d/math_functions.h>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <functional>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
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

class Game
{
public:
    Game();
    ~Game();

    void Init();
    void Run();
    void Destroy();

private:
    bool isInitialized = false;
    bool isDestroyed = false;

    // 窗口和视图
    int width = 1920 / 1.25, height = 1080 / 1.25;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running = true;
    float worldZoom = 1.0f;

    // 输入相关
    SDL_FPoint mousePos = {0.f, 0.f};
    SDL_FPoint lastMousePos = {0.f, 0.f};
    b2Vec2 worldPos = b2Vec2_zero;
    b2Vec2 lastWorldPos = b2Vec2_zero;

    // 时间/FPS
    int tickCount = 0;

    //资源
    TextureManager textureManager;
    SpawnableObjectBrowser spawnableBrowser;

    // 世界与对象
    bool pause = false;
    int particleCount = 0;
    std::vector<b2BodyId> bodyIds;
    b2WorldId worldId;

    float mouseForceScale = 5.0f;
    b2BodyId mouseBodyId = b2_nullBodyId;
    b2JointId mouseJointId = b2_nullJointId;

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
    void HandleEvent(SDL_Event* event);
    void UpdateWorld();
    void DrawWorld();
    void RenderUi();
};
