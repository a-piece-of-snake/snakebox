#include "Game.h"

void Game::Run()
{
    while (window.isOpen())
    {
        Update();
        Render();
        tickCount++;
    }
    return;
}
void Game::Update()
{
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    worldPos = {mousePos.x, mousePos.y};
    if (fpsClock.getElapsedTime().asSeconds() >= 1.0f)
    {
        window.setTitle("CppProject FPS:" +
                        std::to_string(static_cast<unsigned int>(tickCount / fpsClock.getElapsedTime().asSeconds())));
        tickCount = 0;
        fpsClock.restart();
    }
    while (const std::optional event = window.pollEvent())
    {
        ImGui::SFML::ProcessEvent(window, *event);
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        if (!ImGui::IsAnyItemHovered())
            HandleEvent(*event);
    }
    ImGuiRelated();
    if (!ImGui::IsAnyItemHovered())
        if (window.hasFocus())
            KeyLogic();
    UpdateWorld();
}
void Game::Render()
{
    worldView.setSize(static_cast<sf::Vector2f>(window.getSize()));
    window.clear();
    window.setView(worldView);
    DrawWorld();

    ImGui::SFML::Render(window);
    window.display();
}
void Game::ImGuiRelated()
{
    ImGui::SFML::Update(window, deltaClock.restart());

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::Begin("DockSpace", nullptr, window_flags))
    {
        ImGui::PopStyleVar(2);
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id,
                         ImVec2(0.0f, 0.0f),
                         ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
        ImGui::End();
    }

    spawnableBrowser.render();
}
void Game::KeyLogic()
{
}
void Game::HandleEvent(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->scancode == sf::Keyboard::Scan::Num1)
        {
            if (spawnableBrowser.selectedObject[0] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[0]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num2)
        {
            if (spawnableBrowser.selectedObject[1] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[1]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num3)
        {
            if (spawnableBrowser.selectedObject[2] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[2]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num4)
        {
            if (spawnableBrowser.selectedObject[3] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[3]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num5)
        {
            if (spawnableBrowser.selectedObject[4] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[4]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num6)
        {
            if (spawnableBrowser.selectedObject[5] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[5]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num7)
        {
            if (spawnableBrowser.selectedObject[6] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[6]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num8)
        {
            if (spawnableBrowser.selectedObject[7] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[7]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num9)
        {
            if (spawnableBrowser.selectedObject[8] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[8]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scan::Num0)
        {
            if (spawnableBrowser.selectedObject[9] != nullptr)
            {
                if (!spawnableBrowser.selectedObject[9]->spawn(worldPos,
                                                               spawnableBrowser.selectedObjectSize,
                                                               spawnableBrowser.selectedObjectFriction,
                                                               spawnableBrowser.selectedObjectRestitution))
                {
                    ERROR("Failed to spawn object!");
                }
            }
        }
    }
}
void Game::UpdateWorld()
{
    if (!pause)
    {
        b2World_Step(worldId, 1.0f / 15.f, 4);
        particleWorld.update();
    }
    return;
}
void Game::DrawWorld()
{
    drawB2(window, bodyIds);
    drawFluid(window, particleWorld);
}
void Game::RenderUi()
{
}