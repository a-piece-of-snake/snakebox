#pragma once

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <functional>
#include <string>
#include <vector>

#include "ColorfulLog.h"
#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

struct SpawnableObject
{
    std::string name;
    std::string type;
    std::string describe;
    sf::Texture icon;
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
                    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_1))
                    {
                        selectedObject[0] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_2))
                    {
                        selectedObject[1] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_3))
                    {
                        selectedObject[2] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_4))
                    {
                        selectedObject[3] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_5))
                    {
                        selectedObject[4] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_6))
                    {
                        selectedObject[5] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_7))
                    {
                        selectedObject[6] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_8))
                    {
                        selectedObject[7] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_9))
                    {
                        selectedObject[8] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_0))
                    {
                        selectedObject[9] = &obj;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftShift))
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