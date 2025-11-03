/******************************************************************/
/*!
\file      imgui_preferences_window.cpp
\author    Mog Shi Feng
\par       
\date      
\brief     

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"
#include "Configs/ConfigPath.h"

// Serialization
#include <fstream>
#include <RAPIDJSON/document.h>
#include <RAPIDJSON/writer.h>
#include <RAPIDJSON/stringbuffer.h>
#include <RAPIDJSON/prettywriter.h>

void SerializeImVec4(std::string name, ImVec4 data, rapidjson::Value& keys, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value field;
    field.SetString(name.c_str(), allocator);
    keys.AddMember(field, rapidjson::Value().SetObject()
        .AddMember("x", data.x, allocator)
        .AddMember("y", data.y, allocator)
        .AddMember("z", data.z, allocator)
        .AddMember("w", data.w, allocator), allocator);
}

void SerializeImVec2(std::string name, ImVec2 data, rapidjson::Value& keys, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value field;
    field.SetString(name.c_str(), allocator);
    keys.AddMember(field, rapidjson::Value().SetObject()
        .AddMember("x", data.x, allocator)
        .AddMember("y", data.y, allocator), allocator);
}

void SerializeFloat(std::string name, float data, rapidjson::Value& keys, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value field;
    field.SetString(name.c_str(), allocator);
    keys.AddMember(field, data, allocator);
}

void SerializeInt(std::string name, int data, rapidjson::Value& keys, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value field;
    field.SetString(name.c_str(), allocator);
    keys.AddMember(field, data, allocator);
}

void DeserializeImVec4(std::string name, ImVec4& data, const rapidjson::Value& keys) {
    if (keys.HasMember(name.c_str()) && keys[name.c_str()].IsObject()) {
        const rapidjson::Value& vector = keys[name.c_str()];
        if (vector.HasMember("x") && vector["x"].IsFloat()) {
            data.x = vector["x"].GetFloat();
        }
        if (vector.HasMember("y") && vector["y"].IsFloat()) {
            data.y = vector["y"].GetFloat();
        }
        if (vector.HasMember("z") && vector["z"].IsFloat()) {
            data.z = vector["z"].GetFloat();
        }
        if (vector.HasMember("w") && vector["w"].IsFloat()) {
            data.w = vector["w"].GetFloat();
        }
    }
}

void DeserializeImVec2(std::string name, ImVec2& data, const rapidjson::Value& keys) {
    if (keys.HasMember(name.c_str()) && keys[name.c_str()].IsObject()) {
        const rapidjson::Value& vector = keys[name.c_str()];
        if (vector.HasMember("x") && vector["x"].IsFloat()) {
            data.x = vector["x"].GetFloat();
        }
        if (vector.HasMember("y") && vector["y"].IsFloat()) {
            data.y = vector["y"].GetFloat();
        }
    }
}

void DeserializeFloat(std::string name, float& data, const rapidjson::Value& keys) {
    if (keys.HasMember(name.c_str()) && keys[name.c_str()].IsFloat()) {
        data = keys[name.c_str()].GetFloat();
    }
}

void DeserializeInt(std::string name, int& data, const rapidjson::Value& keys) {
    if (keys.HasMember(name.c_str()) && keys[name.c_str()].IsInt()) {
        data = keys[name.c_str()].GetInt();
    }
}

template<typename EnumType>
    requires std::is_enum_v<EnumType>
void DeserializeEnumType(std::string name, EnumType& data, const rapidjson::Value& keys) {
    if (keys.HasMember(name.c_str()) && keys[name.c_str()].IsInt()) {
        data = static_cast<EnumType>(keys[name.c_str()].GetInt());
    }
}

void SerializeProfile(ImGuiStyle* style) {
    if (!style) {
        LOGGING_INFO("No Style Ref");
        return;
    }
    
    rapidjson::Document doc;
    if (!doc.IsArray()) {
        doc.SetArray();  // Initialize as an empty array
    }
    // Holy Shit this is really cursed
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value data(rapidjson::kObjectType);
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        SerializeImVec4(std::string("Gui_Color " + std::to_string(i)), style->Colors[i], data, allocator);
    }
    SerializeImVec2("WindowPadding", style->WindowPadding, data, allocator);
    SerializeImVec2("FramePadding", style->FramePadding, data, allocator);
    SerializeImVec2("ItemSpacing", style->ItemSpacing, data, allocator);
    SerializeImVec2("ItemInnerSpacing", style->ItemInnerSpacing, data, allocator);
    SerializeImVec2("TouchExtraPadding", style->TouchExtraPadding, data, allocator);
    SerializeFloat("IndentSpacing", style->IndentSpacing, data, allocator);
    SerializeFloat("ScrollbarSize", style->ScrollbarSize, data, allocator);
    SerializeFloat("GrabMinSize", style->GrabMinSize, data, allocator);
    SerializeFloat("WindowBorderSize", style->WindowBorderSize, data, allocator);
    SerializeFloat("ChildBorderSize", style->ChildBorderSize, data, allocator);
    SerializeFloat("PopupBorderSize", style->PopupBorderSize, data, allocator);
    SerializeFloat("FrameBorderSize", style->FrameBorderSize, data, allocator);
    SerializeFloat("TabBorderSize", style->TabBorderSize, data, allocator);
    SerializeFloat("TabBarBorderSize", style->TabBarBorderSize, data, allocator);
    SerializeFloat("TabBarOverlineSize", style->TabBarOverlineSize, data, allocator);
    SerializeFloat("WindowRounding", style->WindowRounding, data, allocator);
    SerializeFloat("ChildRounding", style->ChildRounding, data, allocator);
    SerializeFloat("FrameRounding", style->FrameRounding, data, allocator);
    SerializeFloat("PopupRounding", style->PopupRounding, data, allocator);
    SerializeFloat("ScrollbarRounding", style->ScrollbarRounding, data, allocator);
    SerializeFloat("GrabRounding", style->GrabRounding, data, allocator);
    SerializeFloat("TabRounding", style->TabRounding, data, allocator);
    SerializeImVec2("CellPadding", style->CellPadding, data, allocator);
    SerializeFloat("TableAngledHeadersAngle", style->TableAngledHeadersAngle, data, allocator);
    SerializeImVec2("TableAngledHeadersTextAlign", style->TableAngledHeadersTextAlign, data, allocator);
    SerializeImVec2("WindowTitleAlign", style->WindowTitleAlign, data, allocator);
    SerializeInt("WindowMenuButtonPosition", style->WindowMenuButtonPosition, data, allocator);
    SerializeFloat("ColorButtonPosition", static_cast<float>(style->ColorButtonPosition), data, allocator);
    SerializeImVec2("ButtonTextAlign", style->ButtonTextAlign, data, allocator);
    SerializeImVec2("SelectableTextAlign", style->SelectableTextAlign, data, allocator);
    SerializeFloat("SeparatorTextBorderSize", style->SeparatorTextBorderSize, data, allocator);
    SerializeImVec2("SeparatorTextAlign", style->SeparatorTextAlign, data, allocator);
    SerializeImVec2("SeparatorTextPadding", style->SeparatorTextPadding, data, allocator);
    SerializeFloat("LogSliderDeadzone", style->LogSliderDeadzone, data, allocator);
    SerializeFloat("DockingSplitterSize", style->DockingSeparatorSize, data, allocator);
    SerializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayNone", (style->HoverFlagsForTooltipMouse & ImGuiHoveredFlags_DelayNone) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayShort", (style->HoverFlagsForTooltipMouse & ImGuiHoveredFlags_DelayShort) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayNormal", (style->HoverFlagsForTooltipMouse & ImGuiHoveredFlags_DelayNormal) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_Stationary", (style->HoverFlagsForTooltipMouse & ImGuiHoveredFlags_Stationary) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_NoSharedDelay", (style->HoverFlagsForTooltipMouse & ImGuiHoveredFlags_NoSharedDelay) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayNone", (style->HoverFlagsForTooltipNav & ImGuiHoveredFlags_DelayNone) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayShort", (style->HoverFlagsForTooltipNav & ImGuiHoveredFlags_DelayShort) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayNormal", (style->HoverFlagsForTooltipNav & ImGuiHoveredFlags_DelayNormal) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_Stationary", (style->HoverFlagsForTooltipNav & ImGuiHoveredFlags_Stationary) > 0, data, allocator);
    SerializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_NoSharedDelay", (style->HoverFlagsForTooltipNav & ImGuiHoveredFlags_NoSharedDelay) > 0, data, allocator);
    SerializeImVec2("DisplayWindowPadding", style->DisplayWindowPadding, data, allocator);
    SerializeImVec2("DisplaySafeAreaPadding", style->DisplaySafeAreaPadding, data, allocator);
    doc.PushBack(data, allocator);
    
    rapidjson::StringBuffer writeBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(writeBuffer);
    doc.Accept(writer);
    std::ofstream file(configpath::imguiStylePath);
    if (!file.is_open()) {
        LOGGING_WARN("Unable to Save Style Profile");
        file.close();
        return;
    }
    else {
        file << writeBuffer.GetString();
        file.close();
    }
}

void gui::ImGuiHandler::DeserializeProfile() {
    std::ifstream file(configpath::imguiStylePath);
    if (!file.is_open()) {
        LOGGING_WARN("Unable to Open File");
        file.close();
        return;
    }
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    ImGuiStyle& style = ImGui::GetStyle();
    int counter = 0;
    for (int i = 0; i < ImGuiCol_COUNT; i++)
        DeserializeImVec4("Gui_Color " + std::to_string(i), style.Colors[i], doc[0]);
    DeserializeImVec2("WindowPadding", style.WindowPadding, doc[0]);
    DeserializeImVec2("FramePadding", style.FramePadding, doc[0]);
    DeserializeImVec2("ItemSpacing", style.ItemSpacing, doc[0]);
    DeserializeImVec2("ItemInnerSpacing", style.ItemInnerSpacing, doc[0]);
    DeserializeImVec2("TouchExtraPadding", style.TouchExtraPadding, doc[0]);
    DeserializeFloat("IndentSpacing", style.IndentSpacing, doc[0]);
    DeserializeFloat("ScrollbarSize", style.ScrollbarSize, doc[0]);
    DeserializeFloat("GrabMinSize", style.GrabMinSize, doc[0]);
    DeserializeFloat("WindowBorderSize", style.WindowBorderSize, doc[0]);
    DeserializeFloat("ChildBorderSize", style.ChildBorderSize, doc[0]);
    DeserializeFloat("PopupBorderSize", style.PopupBorderSize, doc[0]);
    DeserializeFloat("FrameBorderSize", style.FrameBorderSize, doc[0]);
    DeserializeFloat("TabBorderSize", style.TabBorderSize, doc[0]);
    DeserializeFloat("TabBarBorderSize", style.TabBarBorderSize, doc[0]);
    DeserializeFloat("TabBarOverlineSize", style.TabBarOverlineSize, doc[0]);
    DeserializeFloat("WindowRounding", style.WindowRounding, doc[0]);
    DeserializeFloat("ChildRounding", style.ChildRounding, doc[0]);
    DeserializeFloat("FrameRounding", style.FrameRounding, doc[0]);
    DeserializeFloat("PopupRounding", style.PopupRounding, doc[0]);
    DeserializeFloat("ScrollbarRounding", style.ScrollbarRounding, doc[0]);
    DeserializeFloat("GrabRounding", style.GrabRounding, doc[0]);
    DeserializeFloat("TabRounding", style.TabRounding, doc[0]);
    DeserializeImVec2("CellPadding", style.CellPadding, doc[0]);
    DeserializeFloat("TableAngledHeadersAngle", style.TableAngledHeadersAngle, doc[0]);
    DeserializeImVec2("TableAngledHeadersTextAlign", style.TableAngledHeadersTextAlign, doc[0]);
    DeserializeImVec2("WindowTitleAlign", style.WindowTitleAlign, doc[0]);
    DeserializeEnumType("WindowMenuButtonPosition", style.WindowMenuButtonPosition, doc[0]);
    DeserializeEnumType("ColorButtonPosition", style.ColorButtonPosition, doc[0]);
    DeserializeImVec2("ButtonTextAlign", style.ButtonTextAlign, doc[0]);
    DeserializeImVec2("SelectableTextAlign", style.SelectableTextAlign, doc[0]);
    DeserializeFloat("SeparatorTextBorderSize", style.SeparatorTextBorderSize, doc[0]);
    DeserializeImVec2("SeparatorTextAlign", style.SeparatorTextAlign, doc[0]);
    DeserializeImVec2("SeparatorTextPadding", style.SeparatorTextPadding, doc[0]);
    DeserializeFloat("LogSliderDeadzone", style.LogSliderDeadzone, doc[0]);
    DeserializeFloat("DockingSplitterSize", style.DockingSeparatorSize, doc[0]);
    int flag = 0;
    DeserializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayNone", flag, doc[0]); 
    if (flag) style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_DelayNone;
    DeserializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayShort", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_DelayShort;
    DeserializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_DelayNormal", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_DelayNormal;
    DeserializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_Stationary", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_Stationary;
    DeserializeInt("HoverFlagsForTooltipMouse_ImGuiHoveredFlags_NoSharedDelay", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_NoSharedDelay;
    DeserializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayNone", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_DelayNone;
    DeserializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayShort", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_DelayShort;
    DeserializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_DelayNormal", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_DelayNormal;
    DeserializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_Stationary", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_Stationary;
    DeserializeInt("HoverFlagsForTooltipNav_ImGuiHoveredFlags_NoSharedDelay", flag, doc[0]);
    if (flag) style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_NoSharedDelay;
    DeserializeImVec2("DisplayWindowPadding", style.DisplayWindowPadding, doc[0]);
    DeserializeImVec2("DisplayWindowPadding", style.DisplayWindowPadding, doc[0]);
}

void Style_DefaultWhite(ImGuiStyle* dst = NULL)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    // General window settings
    style->WindowRounding = 5.0f;
    style->FrameRounding = 5.0f;
    style->PopupRounding = 5.0f;
    style->ScrollbarRounding = 5.0f;
    style->GrabRounding = 5.0f;
    style->TabRounding = 5.0f;
    style->WindowBorderSize = 1.0f;
    style->FrameBorderSize = 1.0f;
    style->PopupBorderSize = 1.0f;

    // Setting the colors (Light version)
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f); // Light background
    colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f); // Light frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

    // Accent colors with a soft pastel gray-green
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f); // Soft gray-green for check marks
    colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.70f, 0.60f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f); // Light button background
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f); // Accent color for resize grips
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.70f, 0.60f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.65f, 0.75f, 0.65f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f); // Tabs background
    colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.55f, 0.65f, 0.55f, 1.00f); // Docking preview in gray-green
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

    // Additional styles
    style->FramePadding = ImVec2(8.0f, 4.0f);
    style->ItemSpacing = ImVec2(8.0f, 4.0f);
    style->IndentSpacing = 20.0f;
    style->ScrollbarSize = 16.0f;
}

void CustomStyleProfile(ImGuiStyle* dst = NULL) {
    ImGuiStyle& style = ImGui::GetStyle();
    // Check for current custom profile
    // Will need to serialize color profile myself, ImGui does not provide inate saving of custom color profile.
    // If not using custom, will need to know which color profile is choosen.
    if(ImGui::Button("Save Profile")) {
        SerializeProfile(&style);
    }

    ImGui::SeparatorText("Custom Profile");
    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (ImGui::BeginTabItem("Color")) {
            static int style_idx = 0;
            if (ImGui::Combo("Colors##Selector", &style_idx, "White\0Classic\0Dark\0Light")) // \0Custom+
            {
                switch (style_idx)
                {
                case 0: Style_DefaultWhite();       break;
                case 1: ImGui::StyleColorsClassic(); break;
                case 2: ImGui::StyleColorsDark();   break;
                case 3: ImGui::StyleColorsLight();  break;
                }
            }
            ImGui::Separator();
            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                const char* name = ImGui::GetStyleColorName(i);
                ImGui::PushID(i);
                if (ImGui::Button("?")) ImGui::DebugFlashStyleColor((ImGuiCol)i);
                ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
                ImGui::SameLine();
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar);
                //if (memcmp(&style.Colors[i], &dst->Colors[i], sizeof(ImVec4)) != 0)
                //{
                //    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                //    // so instead of "Save"/"Revert" you'd use icons!
                //    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                //    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { dst->Colors[i] = style.Colors[i]; }
                //    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = dst->Colors[i]; }
                //}
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sizes")) {
            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("##sizes", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

            ImGui::SeparatorText("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

            ImGui::SeparatorText("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 2.0f, "%.0f");
            ImGui::SliderFloat("TabBarOverlineSize", &style.TabBarOverlineSize, 0.0f, 2.0f, "%.0f");

            ImGui::SeparatorText("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tables");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
            ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");

            ImGui::SeparatorText("Widgets");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = (ImGuiDir)(window_menu_button_position - 1);
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat2("SeparatorTextAlign", (float*)&style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("SeparatorTextPadding", (float*)&style.SeparatorTextPadding, 0.0f, 40.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Docking");
            ImGui::SliderFloat("DockingSplitterSize", &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tooltips");
            for (int n = 0; n < 2; n++) {
                if (ImGui::TreeNodeEx(n == 0 ? "HoverFlagsForTooltipMouse" : "HoverFlagsForTooltipNav")) {
                    ImGuiHoveredFlags* p = (n == 0) ? &style.HoverFlagsForTooltipMouse : &style.HoverFlagsForTooltipNav;
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNone", p, ImGuiHoveredFlags_DelayNone);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayShort", p, ImGuiHoveredFlags_DelayShort);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNormal", p, ImGuiHoveredFlags_DelayNormal);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_Stationary", p, ImGuiHoveredFlags_Stationary);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_NoSharedDelay", p, ImGuiHoveredFlags_NoSharedDelay);
                    ImGui::TreePop();
                }
            }
            ImGui::SeparatorText("Misc");
            ImGui::SliderFloat2("DisplayWindowPadding", (float*)&style.DisplayWindowPadding, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            
            ImGui::PopItemWidth();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void gui::ImGuiHandler::DrawPreferencesWindow() {
    if (openPreferencesTab) {
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.3f, ImGui::GetIO().DisplaySize.y * 0.8f), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Preferences", &openPreferencesTab, ImGuiWindowFlags_NoDocking)) {
            CustomStyleProfile();
            ImGui::End();
        }
    }
}