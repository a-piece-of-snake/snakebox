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
    lastMousePos = mousePos;
    lastWorldPos = worldPos;
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
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
    {
        static bool firstClick = true;

        if (firstClick)
        {
            firstClick = false;
        }
        else
        {
            float dx = worldPos.x - lastWorldPos.x;
            float dy = worldPos.y - lastWorldPos.y;
            float distanceSq = dx * dx + dy * dy;

            if (distanceSq > 0.0001f)
            {
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_staticBody;
                b2Vec2 center = {(lastWorldPos.x + worldPos.x) / 2.f, (lastWorldPos.y + worldPos.y) / 2.f};
                bodyDef.position = center;
                b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

                float length = sqrt(distanceSq);
                float angle = atan2(dy, dx);

                b2Polygon line = b2MakeBox(length / 2.f + 2.5f, 10.f);
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                b2CreatePolygonShape(bodyId, &shapeDef, &line);

                b2Body_SetTransform(bodyId, center, b2MakeRot(angle));

                bodyIds.push_back(bodyId);
            }
        }
    }
}
void Game::HandleEvent(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->scancode == sf::Keyboard::Scan::R)
        {
            for (auto& bodyId : bodyIds)
            {
                b2DestroyBody(bodyId);
            }
            bodyIds.clear();
            for (auto& group : particleWorld.groups)
            {
                group.particles.clear();
            }
        }
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
        particleWorld.groups[0].step(1.0f / 15.f);
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