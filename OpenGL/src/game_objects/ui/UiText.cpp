#include "UiText.h"

#include "imgui.h"

void DrawTextOverlay(std::string& text, glm::vec2 position, ImU32 color = IM_COL32(255, 255, 255, 255)) {
   ImDrawList* draw_list = ImGui::GetForegroundDrawList();
   draw_list->AddText(ImVec2(position.x, position.y), color, text.c_str());
}

UiText::UiText(const std::string& text, glm::vec2 position)
   : GameObject(text, DrawPriority::UI, position) {}

void UiText::setUpShader(Renderer& renderer) {}

void UiText::render(Renderer& renderer) {
   DrawTextOverlay(name, position, IM_COL32(255, 255, 255, 255));
}

void UiText::update() {}
