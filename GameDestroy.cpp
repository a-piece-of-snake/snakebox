#include "Game.h"

void Game::Destroy()
{
    b2DestroyWorld(worldId);
    ImGui::SFML::Shutdown();
    return;
}
