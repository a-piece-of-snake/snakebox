#include "Game.h"


Game::Game() : window(sf::VideoMode(sf::Vector2u(width, height)), "CppProject", sf::Style::Default)
{
}

Game::~Game()
{
}

void Game::Init()
{
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

    SUCCESS("Successfully initialized the game!");
    return;
}
void Game::InitImGui()
{
    if (!ImGui::SFML::Init(window))
        ERROR("Failed to initialized Imgui!");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->Clear();

    io.Fonts->AddFontFromFileTTF(
        "Assets/Fonts/NanoDyongSong.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    if (!ImGui::SFML::UpdateFontTexture())
        ERROR("Failed to load font!");


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

    colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.51f, 0.51f, 0.70f);
    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.65f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.10f, 0.16f, 0.89f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.10f, 0.16f, 0.71f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.10f, 0.16f, 0.77f);
    colors[ImGuiCol_Border] = ImVec4(1.00f, 0.75f, 0.75f, 0.34f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.46f, 0.25f, 0.25f, 0.50f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 0.75f, 0.75f, 0.39f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.29f, 0.29f, 0.47f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.78f, 0.32f, 0.36f, 0.34f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.88f, 0.42f, 0.46f, 0.34f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.62f, 0.85f, 1.00f);
    colors[ImGuiCol_TextLink] = ImVec4(0.93f, 0.50f, 0.96f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.93f, 0.70f, 0.70f, 0.40f);
    colors[ImGuiCol_TreeLines] = ImVec4(0.50f, 0.43f, 0.43f, 0.50f);
    colors[ImGuiCol_NavCursor] = ImVec4(0.98f, 0.33f, 0.26f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.84f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.79f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.98f, 0.26f, 0.26f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.94f, 0.20f, 0.20f, 0.89f);
    colors[ImGuiCol_Header] = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.96f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.43f, 0.43f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.10f, 0.10f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.68f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.98f, 0.26f, 0.26f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.93f, 0.31f, 0.31f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.26f, 0.26f, 0.88f);
    colors[ImGuiCol_InputTextCursor] = ImVec4(0.72f, 0.34f, 0.34f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.26f, 0.26f, 0.80f);
    colors[ImGuiCol_Tab] = ImVec4(0.58f, 0.18f, 0.18f, 0.86f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.68f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.42f, 0.14f, 0.14f, 1.00f);

    SUCCESS("Successfully initialized Imgui!");

    return;
}

void Game::InitScene()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    worldId = b2CreateWorld(&worldDef);

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){500.0f, 130.0f};
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(2000.0f, 20.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    bodyIds.push_back(groundId);

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){200.0f, 154.0f};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(10.0f, 10.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.8f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
    bodyIds.push_back(bodyId);


    SUCCESS("Successfully initialized scene!");
    return;
}

void Game::InitWindow()
{
    window.setFramerateLimit(60);
    worldView.setSize(static_cast<sf::Vector2f>(window.getSize()));
    worldView.setCenter({worldView.getSize().x / 2, worldView.getSize().y / 2});
    worldView.rotate(sf::degrees(180));
    SUCCESS("Successfully initialized window!");
    return;
}

void Game::InitResources()
{
    textureManager.get("snake.png"); //Si si si
    textureManager.get("sheep.png");
    {
        SpawnableObject box;
        box.name = "Box";
        box.type = "Solid";
        box.describe = "An ordinary box.";
        box.icon = *textureManager.get("snake.png");
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
        circle.icon = *textureManager.get("snake.png");
        circle.onSpawned = [this](b2Vec2 pos, float size, float friction, float restitution) -> bool {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = pos;
            b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
            b2Circle dynamicCircle;
            dynamicCircle.radius = size;
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
    SUCCESS("Successfully initialized resources!");
    return;
}

void Game::InitFluid()
{
    //Where is the fluid?
    return;
}