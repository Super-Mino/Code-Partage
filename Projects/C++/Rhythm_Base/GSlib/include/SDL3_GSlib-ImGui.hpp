#ifndef _SDL3_GSlib_IMGUI_HPP_
#define _SDL3_GSlib_IMGUI_HPP_

#include <SDL3/SDL.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "ImGui/imgui_impl_sdlrenderer3.h"

namespace gs 
{


bool InitImGui(SDL_Window* win=nullptr, SDL_Renderer* ren=nullptr);

void ImGuiNewFrame();

void RenderImGuiFrame(SDL_Renderer* ren);
void RenderTheImGuiFrame(SDL_Renderer* ren);
void ImGuiRenderFrame(SDL_Renderer* ren);

void QuitImGui();


#endif //_SDL3_GSlib_IMGUI_HPP_


} //namespace gs

