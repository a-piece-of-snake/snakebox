#include <SDL3/SDL_pixels.h>
#include <box2d/box2d.h>
#include <imgui.h>

#include "Fluid.h"
#include "Game.h"
#include "Render.h"

//从box2d示例里直接拿的
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
bool QueryCallback_a(b2ShapeId shapeId, void* context)
{
    QueryContext* queryContext = static_cast<QueryContext*>(context);

    b2BodyId bodyId = b2Shape_GetBody(shapeId);
    b2BodyType bodyType = b2Body_GetType(bodyId);
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

    // float hue = fmodf(SDL_GetTicks() / 10.0f, 360.0f);
    // SDL_FColor outlineColor = HSLAtoRGBA_F(hue, 1.f, 0.5f);
    // SDL_FColor bodyColor = HSLAtoRGBA_F(hue, 1.f, 0.5f, 0.5f);
    // getColorStyle()[ColorKeys::b2BodyOutline] = outlineColor;
    // getColorStyle()[ColorKeys::b2Body] = bodyColor;

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
    if (B2_IS_NON_NULL(mouseBodyId) && B2_IS_NON_NULL(mouseJointId))
    {
        std::vector<SDL_Vertex> mouseLine;
        b2Rot mouseBodyRot = b2Body_GetRotation(b2Joint_GetBodyB(mouseJointId));
        float cosAngle = mouseBodyRot.c;
        float sinAngle = mouseBodyRot.s;
        b2Vec2 mouseBodyPos = b2Body_GetPosition(b2Joint_GetBodyB(mouseJointId));
        b2Vec2 mouseBodyLocal = b2Joint_GetLocalAnchorB(mouseJointId);
        makeLine(&mouseLine,
                 mousePos,
                 {mouseBodyPos.x + mouseBodyLocal.x * cosAngle - mouseBodyLocal.y * sinAngle,
                  mouseBodyPos.y + mouseBodyLocal.x * sinAngle + mouseBodyLocal.y * cosAngle},
                 2.5f,
                 getColorStyle()[ColorKeys::b2Joint]);
        SDL_RenderGeometry(renderer, nullptr, mouseLine.data(), mouseLine.size(), nullptr, 0);
    }
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

    //临时放在这里
    if (ImGui::Begin("Style Editor"))
    {
        auto& style = getColorStyle();
        const char* keyNames[] = {"Body Color", "Outline Color", "Joint Color"};

        static SDL_FColor originalColor;
        static int tick = -1;
        static int activeHelpId = -1;

        if (tick >= 0)
            tick--;

        for (int i = 0; i < ColorKeys::COUNT; ++i)
        {
            ImGui::PushID(i);

            if (ImGui::Button("?"))
            {
                if (activeHelpId != -1)
                {
                    style[activeHelpId] = originalColor;
                }

                activeHelpId = i;
                tick = 120;
                originalColor = style[i];
            }

            ImGui::SameLine();

            float* targetColor = (activeHelpId == i) ? &originalColor.r : &style[i].r;

            if (ImGui::ColorEdit4(keyNames[i], targetColor))
            {
            }

            if (activeHelpId == i)
            {
                if (tick > 0)
                {
                    float hue = fmodf(tick * 3.f, 360.0f);
                    style[i] = HSLAtoRGBA_F(hue, 1.f, 0.5f);
                }
                else if (tick == 0)
                {
                    style[i] = originalColor;
                    activeHelpId = -1;
                }
            }

            ImGui::PopID();
        }
    }
    ImGui::End();

    static bool show_exit_confirm = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {
            }
            if (ImGui::MenuItem("Redo"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
            }
            if (ImGui::MenuItem("Open"))
            {
            }
            if (ImGui::MenuItem("Save"))
            {
            }
            if (ImGui::MenuItem("Exit"))
            {
                show_exit_confirm = true;
                ImGui::SetNextWindowPos(ImGui::GetMousePos());
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
    if (show_exit_confirm)
    {
        if (ImGui::Begin("Exit Confirm", &show_exit_confirm, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("ARE YOU SURE?");
            if (ImGui::Button("Yes"))
            {
                running = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                show_exit_confirm = false;
            }
        }
        ImGui::End();
    }
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

    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT) && keyStates[SDL_SCANCODE_D])
    {
        b2Vec2 p = worldPos;
        // Make a small box.
        b2AABB box;
        b2Vec2 d = {0.001f, 0.001f};
        box.lowerBound = b2Sub(p, d);
        box.upperBound = b2Add(p, d);

        // Query the world for overlapping shapes.
        QueryContext queryContext = {p, b2_nullBodyId};
        if (keyStates[SDL_SCANCODE_A])
            b2World_OverlapAABB(worldId, box, b2DefaultQueryFilter(), QueryCallback_a, &queryContext);
        else
            b2World_OverlapAABB(worldId, box, b2DefaultQueryFilter(), QueryCallback, &queryContext);

        if (B2_IS_NON_NULL(queryContext.bodyId))
        {
            bodyDestroyBuffer.push_back(queryContext.bodyId);
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
            break;
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
        if (B2_IS_NON_NULL(mouseBodyId))
        {
            if (!b2Body_IsValid(mouseBodyId) || (B2_IS_NON_NULL(mouseJointId) && !b2Joint_IsValid(mouseJointId)))
            {
                mouseJointId = b2_nullJointId;
                mouseBodyId = b2_nullBodyId;
            }
            else
            {
                b2Body_SetTargetTransform(mouseBodyId, {worldPos, b2Rot_identity}, 1.0f / 15.0f);
            }
        }
        particleWorld.groups[0].step(1.0f / 15.f);
        b2World_Step(worldId, 1.0f / 15.f, 4);
        particleWorld.update();
    }

    if (!bodyDestroyBuffer.empty())
    {
        for (auto& bodyId : bodyDestroyBuffer)
        {
            if (b2Body_IsValid(bodyId))
            {
                if (B2_IS_NON_NULL(mouseJointId) && B2_ID_EQUALS(b2Joint_GetBodyB(mouseJointId), bodyId))
                {
                    b2DestroyJoint(mouseJointId);
                    mouseJointId = b2_nullJointId;
                }

                std::erase_if(bodyIds, [&](b2BodyId id) { return B2_ID_EQUALS(id, bodyId); });
                for (auto& group : particleWorld.groups)
                {
                    std::erase_if(group.particles, [&](Particle p) { return B2_ID_EQUALS(p.bodyId, bodyId); });
                }
                b2DestroyBody(bodyId);
            }
        }
        bodyDestroyBuffer.clear();
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
