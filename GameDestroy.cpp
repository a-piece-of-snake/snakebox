#include "Game.h"

void Game::Destroy()
{
    if (isDestroyed)
    {
        WARN("Dont destroy twice!");
        return;
    }
    isDestroyed = true;
    b2DestroyWorld(worldId);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    SUCCESS("Successfully destroy the game!");
    return;
}
