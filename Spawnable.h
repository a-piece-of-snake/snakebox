#pragma once

struct SDL_Texture;

#include <box2d/box2d.h>
#include <functional>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <string>
#include <vector>

#include "ColorfulLog.h"

struct SpawnableObject
{
    std::string name;
    std::string type;
    std::string describe;
    SDL_Texture* icon;
    unsigned int ID;
    std::function<bool(b2Vec2 pos, float size, float friction, float restitution)> onSpawned;
    bool spawn(b2Vec2 pos, float size, float friction, float restitution)
    {
        if (onSpawned)
        {
            return onSpawned(pos, size, friction, restitution);
        }
        ERROR("Failed to spawn %s !", name.c_str());

        return false;
    }
};

struct SpawnableObjectBrowser
{
    std::vector<SpawnableObject> objects;
    float iconSize = 50.f;
    SpawnableObject* selectedObject[11];
    float selectedObjectSize = 10.f;
    float selectedObjectFriction = 0.5f;
    float selectedObjectRestitution = 0.1f;
    float selectedObjectDensity = 1.0f;
    char searchBuffer[1024];

    std::string search = "";

    void addObject(SpawnableObject obj)
    {
        objects.emplace_back(std::move(obj));
    }

    void clearObjects()
    {
        objects.clear();
    }

    void render()
    {
        if (ImGui::Begin("Spawnable Objects Browser"))
        {
            if (ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer)))
            {
                search = std::string(searchBuffer);
            }
            ImGui::NewLine();
            ImGui::Separator();

            ImGui::Text("Objects :");
            ImGui::NewLine();

            ImGui::BeginGroup();

            float total_width = ImGui::GetContentRegionAvail().x;
            float button_width = iconSize + ImGui::GetStyle().ItemSpacing.x * 2;
            int buttons_per_row = (total_width + ImGui::GetStyle().ItemSpacing.x) / button_width;

            if (buttons_per_row == 0)
            {
                buttons_per_row = 1;
            }

            int index = 0;
            for (auto& obj : objects)
            {
                if (obj.name.find(search) == std::string::npos)
                    continue;
                if (ImGui::ImageButton((obj.name + std::to_string(index)).c_str(), obj.icon, {iconSize, iconSize}))
                {
                    //selectedObject[index] = &obj;
                }

                if (ImGui::IsItemHovered())
                {
                    if (ImGui::BeginItemTooltip())
                    {
                        ImGui::Text("%s", obj.name.c_str());
                        ImGui::Separator();
                        ImGui::Text("%s", obj.describe.c_str());

                        ImGui::EndTooltip();
                    }
                    static const ImGuiKey numKeys[] = {ImGuiKey_1,
                                                       ImGuiKey_2,
                                                       ImGuiKey_3,
                                                       ImGuiKey_4,
                                                       ImGuiKey_5,
                                                       ImGuiKey_6,
                                                       ImGuiKey_7,
                                                       ImGuiKey_8,
                                                       ImGuiKey_9,
                                                       ImGuiKey_0};
                    for (int i = 0; i < 10; ++i)
                    {
                        if (ImGui::IsKeyPressed(numKeys[i]))
                        {
                            selectedObject[i] = &obj;
                            break;
                        }
                    }
                    if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
                    {
                        selectedObject[10] = &obj;
                    }
                }
                /*
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                INFO("right");
            }
            */
                if ((index + 1) % buttons_per_row != 0 && (index + 1) < objects.size())
                {
                    ImGui::SameLine();
                }
                index++;
            }

            ImGui::EndGroup();
        }
        ImGui::End();
    }
};