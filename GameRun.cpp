#include "Game.h"


struct QueryContext
{
    b2Vec2 point;
    b2BodyId bodyId = b2_nullBodyId;
};

bool QueryCallback(b2ShapeId shapeId, void* context)
{
    QueryContext* queryContext = static_cast<QueryContext*>(context);

    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    b2BodyType bodyType = b2Body_GetType(bodyId);
    if (bodyType != b2_dynamicBody)
    {
        // continue query
        return true;
    }

    bool overlap = b2Shape_TestPoint(shapeId, queryContext->point);
    if (overlap)
    {
        // found shape
        queryContext->bodyId = bodyId;
        return false;
    }

    return true;
}

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
    lastOMousePos = oMousePos;
    lastMousePos = mousePos;
    lastWorldPos = worldPos;
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    oMousePos = sf::Mouse::getPosition();
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
    worldView.setSize({window.getSize().x * worldZoom, window.getSize().y * worldZoom});
    window.clear();
    window.setView(uiView);
    //uiView.setSize(static_cast<sf::Vector2f>(window.getSize()));


    uiView.setSize(static_cast<sf::Vector2f>(window.getSize()));
    uiView.setCenter(uiView.getSize() / 2.f);

    uiView.setScissor(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
    sf::Texture* bgT = textureManager.get("BackGround.png").get();
    bgT->setRepeated(true);
    sf::RectangleShape background(static_cast<sf::Vector2f>(uiView.getSize()));

    sf::Vector2f viewSize = uiView.getSize();
    sf::Vector2i rectSize(static_cast<int>(viewSize.x * worldZoom), static_cast<int>(viewSize.y * worldZoom));

    sf::Vector2i rectPos(static_cast<int>(worldView.getCenter().x - rectSize.x / 2),
                         static_cast<int>(worldView.getCenter().y - rectSize.y / 2));

    background.setTexture(bgT);
    background.setTextureRect(sf::IntRect(rectPos, rectSize));

    window.draw(background);

    // sf::RenderTexture bgTexture({window.getSize().x / 2, window.getSize().y / 2});
    // auto noiseShader = shaderManager.get("noise").get();
    // noiseShader->setUniform("u_resolution", static_cast<sf::Vector2f>(window.getSize()));
    // noiseShader->setUniform("u_time", clock.getElapsedTime().asSeconds());
    // bgTexture.draw(background, noiseShader);
    // sf::Sprite bgSprite(bgTexture.getTexture());
    // bgSprite.scale({10, 10});
    // window.draw(bgSprite);

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
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (B2_IS_NON_NULL(mouseJointId))
        {
            return;
        }
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            b2Vec2 p = worldPos;
            // Make a small box.
            b2AABB box;
            b2Vec2 d = {0.001f, 0.001f};
            box.lowerBound = b2Sub(p, d);
            box.upperBound = b2Add(p, d);

            // Query the world for overlapping shapes.
            QueryContext queryContext = {p, b2_nullBodyId};
            b2World_OverlapAABB(worldId, box, b2DefaultQueryFilter(), QueryCallback, &queryContext);

            if (B2_IS_NON_NULL(queryContext.bodyId))
            {
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_kinematicBody;
                bodyDef.position = worldPos;
                bodyDef.enableSleep = false;
                mouseBodyId = b2CreateBody(worldId, &bodyDef);
                b2MotorJointDef jointDef = b2DefaultMotorJointDef();
                jointDef.base.bodyIdA = mouseBodyId;
                jointDef.base.bodyIdB = queryContext.bodyId;
                jointDef.base.localFrameB.p = b2Body_GetLocalPoint(queryContext.bodyId, p);
                jointDef.linearHertz = 1.5f;
                jointDef.linearDampingRatio = 2.5f;

                b2MassData massData = b2Body_GetMassData(queryContext.bodyId);
                float g = b2Length(b2World_GetGravity(worldId));
                float mg = massData.mass * g;

                jointDef.maxSpringForce = mouseForceScale * mg;

                if (massData.mass > 0.0f)
                {
                    // This acts like angular friction
                    float lever = sqrtf(massData.rotationalInertia / massData.mass);
                    jointDef.maxVelocityTorque = 0.25f * lever * mg;
                }

                mouseJointId = b2CreateMotorJoint(worldId, &jointDef);
            }
        }
    }
    if (const auto* mouseRelease = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseRelease->button == sf::Mouse::Button::Left)
        {
            if (B2_IS_NON_NULL(mouseJointId))
            {
                b2DestroyJoint(mouseJointId, true);
                mouseJointId = b2_nullJointId;

                b2DestroyBody(mouseBodyId);
                mouseBodyId = b2_nullBodyId;
            }
        }
    }
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
        static const std::unordered_map<sf::Keyboard::Scan, int> keyToIndex = {{sf::Keyboard::Scan::Num1, 0},
                                                                               {sf::Keyboard::Scan::Num2, 1},
                                                                               {sf::Keyboard::Scan::Num3, 2},
                                                                               {sf::Keyboard::Scan::Num4, 3},
                                                                               {sf::Keyboard::Scan::Num5, 4},
                                                                               {sf::Keyboard::Scan::Num6, 5},
                                                                               {sf::Keyboard::Scan::Num7, 6},
                                                                               {sf::Keyboard::Scan::Num8, 7},
                                                                               {sf::Keyboard::Scan::Num9, 8},
                                                                               {sf::Keyboard::Scan::Num0, 9}};

        auto it = keyToIndex.find(keyPressed->scancode);
        if (it != keyToIndex.end())
        {
            int index = it->second;
            auto* obj = spawnableBrowser.selectedObject[index];

            if (obj && !obj->spawn(worldPos,
                                   spawnableBrowser.selectedObjectSize,
                                   spawnableBrowser.selectedObjectFriction,
                                   spawnableBrowser.selectedObjectRestitution))
            {
                ERROR("Failed to spawn object!");
            }
        }
    }
    if (const auto* MouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        float zoom = (MouseWheelScrolled->delta > 0) ? -0.1f : 0.1f;
        worldZoom += zoom;
        worldZoom = std::max(worldZoom, 0.01f);
    }
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (b2Joint_IsValid(mouseJointId) == false)
        {
            // The world or attached body was destroyed.
            mouseJointId = b2_nullJointId;
        }

        mousePos = {(float)mouseMoved->position.x, (float)mouseMoved->position.y};
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt))
        {
            worldView.move({static_cast<float>(lastOMousePos.x - oMousePos.x) * worldZoom,
                            static_cast<float>(lastOMousePos.y - oMousePos.y) * worldZoom});
        }
    }
}
void Game::UpdateWorld()
{
    if (!pause)
    {
        if (B2_IS_NON_NULL(mouseJointId) && b2Joint_IsValid(mouseJointId) == false)
        {
            // The world or attached body was destroyed.
            mouseJointId = b2_nullJointId;

            if (B2_IS_NON_NULL(mouseBodyId))
            {
                b2DestroyBody(mouseBodyId);
                mouseBodyId = b2_nullBodyId;
            }
        }

        if (B2_IS_NON_NULL(mouseBodyId) && 1.0 / 15.f > 0.0f)
        {
            b2Body_SetTargetTransform(mouseBodyId, {worldPos, b2Rot_identity}, 1.0 / 15.f);
        }

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