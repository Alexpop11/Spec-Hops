#include "Text.h"

#include "imgui.h"

void DrawTextOverlay(std::string& text, glm::vec2 position, ImU32 color = IM_COL32(255, 255, 255, 255)) {
   ImDrawList* draw_list = ImGui::GetForegroundDrawList();
   draw_list->AddText(ImVec2(position.x, position.y), color, text.c_str());
}

Text::Text(const std::string& text, ImFont* font, glm::vec2 position)
   : GameObject(text, DrawPriority::UI, position)
   , font(font) {}

void Text::setUpShader(Renderer& renderer) {}

void Text::render(Renderer& renderer) {
   ImGui::PushFont(font);
   DrawTextOverlay(name, position, IM_COL32(255, 255, 255, 255));
   ImGui::PopFont();
}

void Text::update() {}
