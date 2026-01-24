#include "Game.h"

//从box1d示例里直接拿的
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
    while (running)
    {
        Update();
        Render();
        tickCount++;
    }
    return;
}
void Game::Update()
{
    lastMousePos = mousePos;
    lastWorldPos = worldPos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    worldPos = {mousePos.x, mousePos.y};

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
        if (!ImGui::IsAnyItemHovered())
            HandleEvent(&event);
    }

    ImGuiRelated();
    if (!ImGui::IsAnyItemHovered())
        KeyLogic();
    UpdateWorld();
}
void renderMouseTriangle(SDL_Renderer* renderer, SDL_FPoint mousePos)
{
    float time = SDL_GetTicks() / 1000.0f;
    float angle = time * 2.0f;
    float radius = 30.0f;
    float alpha = 0.5f;
    SDL_Vertex mouseVertices[3] = {
        {{mousePos.x + radius * cosf(angle), mousePos.y + radius * sinf(angle)}, {1.0f, 0.0f, 0.0f, alpha}, {0, 0}},
        {{mousePos.x + radius * cosf(angle + 2.094f), mousePos.y + radius * sinf(angle + 2.094f)},
         {0.0f, 1.0f, 0.0f, alpha},
         {0, 0}},
        {{mousePos.x + radius * cosf(angle + 4.188f), mousePos.y + radius * sinf(angle + 4.188f)},
         {0.0f, 0.0f, 1.0f, alpha},
         {0, 0}}};
    SDL_RenderGeometry(renderer, nullptr, mouseVertices, 3, nullptr, 0);
}
void Game::Render()
{
    SDL_SetRenderDrawColorFloat(renderer, 12.f / 255, 12.f / 255, 18.f / 255, 0.7f);
    SDL_RenderClear(renderer);
    DrawWorld();
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    renderMouseTriangle(renderer, mousePos);
    SDL_RenderPresent(renderer);
}
void Game::ImGuiRelated()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();


    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
    //                                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    //                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
    //                                 ImGuiWindowFlags_NoBackground;

    // ImGuiViewport* viewport = ImGui::GetMainViewport();
    // ImGui::SetNextWindowPos(viewport->WorkPos);
    // ImGui::SetNextWindowSize(viewport->WorkSize);
    // ImGui::SetNextWindowViewport(viewport->ID);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    // if (ImGui::Begin("DockSpace", nullptr, window_flags))
    // {
    //     ImGui::PopStyleVar(2);
    //     ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    //     ImGui::DockSpace(dockspace_id,
    //                      ImVec2(0.0f, 0.0f),
    //                      ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
    //     ImGui::End();
    // }

    spawnableBrowser.render();
}
void Game::KeyLogic()
{
    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT) && keyStates[SDL_SCANCODE_LCTRL])
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
void Game::HandleEvent(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (B2_IS_NON_NULL(mouseJointId))
        {
            return;
        }
        if (event->button.button == SDL_BUTTON_LEFT)
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
                //为啥aur里装的box2d
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_kinematicBody;
                bodyDef.position = worldPos;
                bodyDef.enableSleep = false;
                mouseBodyId = b2CreateBody(worldId, &bodyDef);
                b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();
                jointDef.bodyIdA = mouseBodyId;
                jointDef.bodyIdB = queryContext.bodyId;
                jointDef.localAnchorA.x = b2Body_GetLocalPoint(mouseBodyId, p).x;
                jointDef.localAnchorA.y = b2Body_GetLocalPoint(mouseBodyId, p).y;
                jointDef.localAnchorB.x = b2Body_GetLocalPoint(queryContext.bodyId, p).x;
                jointDef.localAnchorB.y = b2Body_GetLocalPoint(queryContext.bodyId, p).y;
                jointDef.length = 0.001f;
                jointDef.enableMotor = true;
                jointDef.enableSpring = true;
                jointDef.motorSpeed = 10.f;
                jointDef.hertz = 0.1f;
                jointDef.dampingRatio = 0.5f;
                //jointDef.linearHertz = 1.5f;
                //jointDef.linearDampingRatio = 2.5f;

                mouseJointId = b2CreateDistanceJoint(worldId, &jointDef);
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (B2_IS_NON_NULL(mouseJointId))
            {
                b2DestroyJoint(mouseJointId);
                mouseJointId = b2_nullJointId;

                b2DestroyBody(mouseBodyId);
                mouseBodyId = b2_nullBodyId;
            }
        }
        break;
    }
    case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = event->key.key;
        if (key == SDLK_R)
        {
            SUCCESS("World Reset");
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
        if (key == SDLK_SPACE)
        {
            pause = !pause;
        }
        if (key >= SDLK_0 && key <= SDLK_9)
        {
            int index = key - SDLK_0 - 1;
            if (key == SDLK_0)
            {
                index = 9;
            }
            if (index < 11)
            {
                auto* obj = spawnableBrowser.selectedObject[index];
                if (obj)
                {
                    bool success = obj->spawn(worldPos,
                                              spawnableBrowser.selectedObjectSize,
                                              spawnableBrowser.selectedObjectFriction,
                                              spawnableBrowser.selectedObjectRestitution);
                    if (!success)
                        ERROR("Failed to spawn object %d!", index);
                }
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_WHEEL: {
        float zoom = (event->wheel.y > 0) ? -0.1f : 0.1f;
        worldZoom += zoom;
        worldZoom = std::max(worldZoom, 0.01f);
        break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
        if (b2Joint_IsValid(mouseJointId) == false)
        {
            // The world or attached body was destroyed.
            mouseJointId = b2_nullJointId;
        }
        const bool* keyStates = SDL_GetKeyboardState(nullptr);
        Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
        if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE) || keyStates[SDL_SCANCODE_LALT])
        {
            // worldView.move({static_cast<float>(lastOMousePos.x - oMousePos.x) * worldZoom,
            //                 static_cast<float>(lastOMousePos.y - oMousePos.y) * worldZoom});
        }
        break;
    }
    default: {
        break;
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
    drawB2(renderer, bodyIds);
    drawFluid(renderer, particleWorld);
}
void Game::RenderUi()
{
}
