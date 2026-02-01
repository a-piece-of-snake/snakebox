#include "Game.h"

Game::Game()
{
    window = nullptr;
    renderer = nullptr;
    running = true;
    worldZoom = 1.0f;
    worldId = b2_nullWorldId;
    mouseJointId = b2_nullJointId;
    mouseBodyId = b2_nullBodyId;

    // 初始化其他成员变量（根据你的 Game.h）
    tickCount = 0;
    pause = false;
    particleCount = 0;
}
Game::~Game()
{
    Destroy();
}

void Game::Init()
{
    if (isInitialized)
    {
        WARN("Dont init twice!");
        return;
    }
    isInitialized = true;
    try
    {
        std::locale loc("zh_CN.UTF-8");
        std::wcout.imbue(loc);
    }
    catch (const std::runtime_error& e)
    {
        std::wcout.imbue(std::locale::classic());
    }
    InitWindow();

    InitImGui();

    InitResources();

    InitFluid();

    InitScene();

    SUCCESS("Successfully init the game!");
    return;
}
void Game::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->Clear();

    io.Fonts->AddFontFromFileTTF(
        "Assets/Fonts/NanoDyongSong.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    ImGui::GetStyle().ChildRounding = 12;
    ImGui::GetStyle().WindowRounding = 12;
    ImGui::GetStyle().FrameRounding = 12;
    ImGui::GetStyle().PopupRounding = 12;
    ImGui::GetStyle().ScrollbarRounding = 12;
    ImGui::GetStyle().TabRounding = 12;
    ImGui::GetStyle().TreeLinesRounding = 12;
    ImGui::GetStyle().GrabRounding = 12;

    ImGui::GetStyle().FrameBorderSize = 1;

    ImGui::GetStyle().SeparatorTextAlign.x = 0.5f;

    ImVec4* colors = ImGui::GetStyle().Colors;

    //colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.51f, 0.51f, 0.70f);
    ImVec4 bg = ImVec4(0.12f, 0.12f, 0.18f, 0.85f);
    ImVec4 fg = ImVec4(0.82f, 0.53f, 0.64f, 1.00f);
    ImVec4 sub_fg = ImVec4(0.69f, 0.55f, 0.70f, 1.00f);
    ImVec4 border = ImVec4(0.30f, 0.34f, 0.42f, 0.50f);

    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.47f, 0.55f, 1.00f);
    colors[ImGuiCol_WindowBg] = bg;
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.95f);
    colors[ImGuiCol_Border] = border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.26f, 0.32f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.53f, 0.64f, 0.20f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.18f, 0.50f);

    colors[ImGuiCol_Button] = ImVec4(0.82f, 0.53f, 0.64f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = fg;
    colors[ImGuiCol_ButtonActive] = ImVec4(0.82f, 0.53f, 0.64f, 0.80f);

    colors[ImGuiCol_Header] = ImVec4(0.69f, 0.55f, 0.70f, 0.30f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.69f, 0.55f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderActive] = sub_fg;

    colors[ImGuiCol_CheckMark] = fg;
    colors[ImGuiCol_SliderGrab] = ImVec4(0.82f, 0.53f, 0.64f, 0.70f);
    colors[ImGuiCol_SliderGrabActive] = fg;

    colors[ImGuiCol_Tab] = ImVec4(0.23f, 0.26f, 0.32f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.82f, 0.53f, 0.64f, 0.80f);
    colors[ImGuiCol_TabSelected] = fg;
    colors[ImGuiCol_TabSelectedOverline] = fg;
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.82f, 0.53f, 0.64f, 0.20f);

    colors[ImGuiCol_Separator] = border;
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.82f, 0.53f, 0.64f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = fg;
    colors[ImGuiCol_ResizeGripActive] = fg;


    SUCCESS("Successfully init Imgui!");

    return;
}

void Game::InitScene()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    worldId = b2CreateWorld(&worldDef);

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){500.0f, 730.0f};
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(2000.0f, 20.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    bodyIds.push_back(groundId);

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){200.0f, 715.0f};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(10.0f, 10.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.8f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
    bodyIds.push_back(bodyId);


    SUCCESS("Successfully init scene!");
    return;
}

void Game::InitWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
    {
        SDL_Log("Failed to init SDL!: %s", SDL_GetError());
        ERROR("Failed to init SDL!");
    }
    window = SDL_CreateWindow("CppProject", width, height, SDL_WINDOW_TRANSPARENT);
    if (!window)
        ERROR("Failed to init window!");
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
        ERROR("Failed to init renderer!");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SUCCESS("Successfully init window!");
    return;
}

void Game::InitResources()
{
    textureManager.init(renderer);
    textureManager.get(renderer, "snake.png"); //Si si si
    textureManager.get(renderer, "sheep.png");
    {
        SpawnableObject box;
        box.name = "Box";
        box.type = "Solid";
        box.describe = "An ordinary box.";
        box.icon = textureManager.get(renderer, "snake.png").get();
        box.onSpawned = [this](b2Vec2 pos, float size, float friction, float restitution) -> bool {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = pos;
            b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
            b2Polygon dynamicBox = b2MakeBox(size, size);
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.material.friction = friction;
            shapeDef.material.restitution = restitution;
            b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
            bodyIds.push_back(bodyId);

            return true;
        };
        spawnableBrowser.addObject(box);
    }
    {
        SpawnableObject circle;
        circle.name = "Cirlce";
        circle.type = "Solid";
        circle.describe = "An ordinary circle.";
        circle.icon = textureManager.get(renderer, "snake.png").get();
        circle.onSpawned = [this](b2Vec2 pos, float size, float friction, float restitution) -> bool {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = pos;
            b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
            b2Circle dynamicCircle;
            dynamicCircle.radius = size;
            dynamicCircle.center = b2Vec2_zero;
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.material.friction = friction;
            shapeDef.material.restitution = restitution;
            b2CreateCircleShape(bodyId, &shapeDef, &dynamicCircle);
            bodyIds.push_back(bodyId);

            return true;
        };
        spawnableBrowser.addObject(circle);
    }
    {
        SpawnableObject particle;
        particle.name = "particle";
        particle.type = "particle";
        particle.describe = "An ordinary circle.";
        particle.icon = textureManager.get(renderer, "snake.png").get();
        particle.onSpawned = [this](b2Vec2 pos, float size, float friction, float restitution) -> bool {
            particleWorld.groups[0].createParticle(worldId, pos);

            return true;
        };
        spawnableBrowser.addObject(particle);
    }
    SUCCESS("Successfully init resources!");
    return;
}

void Game::InitFluid()
{
    ParticleGroup group;
    group.world = &particleWorld;
    group.color = SDL_Color{0, 255, 255};
    particleWorld.groups.push_back(std::move(group));
    particleWorld.init();
    SUCCESS("Successfully init fluid!");
    return;
}
