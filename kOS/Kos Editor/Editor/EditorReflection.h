/******************************************************************/
/*!
\file      EditorReflection.h
\author    Jaz Winn Ng
\par       jazwinn.ng@digipen.edu
\date      Sept 28, 2025
\brief     Reflection for editor to draw components dynamically.
         - DrawComponents: Templated struct to draw various component fields in the editor using ImGui.
            - operator() overloads for different data types (float, int, enum, glm::vec2, glm::vec3, glm::vec4).
            - Uses ImGui functions to create UI elements for editing component fields.
			- Supports enum types using magic_enum library for better type safety and usability.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/
#pragma once
#include "config/pch.h"
#include "Editor/Payload.h"

constexpr float sameLineParam = 200.0f; //Padding for the slider


template <typename T>
struct DrawComponents {

    T m_Array;
    int count{};



    void operator()(float& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam); 
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        std::string id = "##" + m_Array[count];

        ImGui::DragFloat(id.c_str(), &_args, 0.1f, -0.0f, 0.0f, "%.2f");

        count++;

    }

    void operator()(int& _args) {

        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        std::string title = "##" + m_Array[count];
        ImGui::DragInt(title.c_str(), &_args, 1.0f);

        count++;

    }

    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    void operator()(EnumType& _args) {

        constexpr auto enumNames = magic_enum::enum_names<EnumType>();

        // Persistent storage for null-terminated strings
        static std::array<std::string, enumNames.size()> namesStr{};
        static std::array<const char*, enumNames.size()> items{};

        static bool initialized = false;
        if (!initialized) {
            for (size_t i = 0; i < enumNames.size(); i++) {
                namesStr[i] = std::string(enumNames[i]);  // store the string
                items[i] = namesStr[i].c_str();           // pointer is valid now
            }
            initialized = true;
        }

        int current = static_cast<int>(_args);
        ImGui::Text(m_Array[count].c_str());      // label
        ImGui::SameLine(sameLineParam);     // move to x=150 (or some offset)
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x); // width of the combo box
        if (ImGui::Combo(std::string("##" + m_Array[count]).c_str(), &current, items.data(), (int)items.size())) {
            _args = static_cast<EnumType>(current);
        }

        count++;
    }

    void operator()(glm::vec2& _args) {


        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);



        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x/2);
        std::string title = "##X" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.x, 0.1f, 0.0f, 0.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        title = "##Y" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.y, 0.1f, -1000.0f, 1000.0f, "%.2f");


        count++;

    }

    void operator()(glm::vec3& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);



        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
        std::string title = "##X" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.x, 0.1f, 0.0f, 0.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x/2);
        title = "##Y" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.y, 0.1f, 0.0f, 0.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        title = "##Z" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.z, 0.1f, 0.0f, 0.0f, "%.2f");

        count++;

    }

    void operator()(glm::vec4& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);



        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 4);
        std::string title = "##X" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.x, 0.1f, 0.0f, 0.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
        title = "##Y" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.y, 0.1f, 0.0f, 0.0f, "%.2f");

        ImGui::SameLine();
        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
        title = "##Z" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.z, 0.1f, 0.0f, 0.0f, "%.2f");


        ImGui::SameLine();
        ImGui::Text("W");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        title = "##W" + m_Array[count];
        ImGui::DragFloat(title.c_str(), &_args.w, 0.1f, 0.0f, 0.0f, "%.2f");

        count++;

    }


    void operator()(bool& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);
        std::string title = "##" + m_Array[count];
        ImGui::Checkbox(title.c_str(), &_args);

        count++;

    }

    void operator()(std::string& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);
        std::string title = "##" + m_Array[count];


        static char buffer[256];
        std::strncpy(buffer, _args.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        // InputText returns true only when Enter is pressed
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText(title.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Update your string only once
            _args = buffer;
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file")) {
                IM_ASSERT(payload->DataSize == sizeof(AssetPayload));
                const AssetPayload* data = static_cast<const AssetPayload*>(payload->Data);

                _args = data->path;

            }
            ImGui::EndDragDropTarget();
        }

        count++;
    }

    void operator()(utility::GUID& _args) {
        ImGui::Text(m_Array[count].c_str());
        ImGui::SameLine(sameLineParam);
        std::string title = "##" + m_Array[count];


        static char buffer[256];
        std::strncpy(buffer, _args.GetToString().c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        // InputText returns true only when Enter is pressed
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText(title.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Update your string only once
            _args.SetFromString(buffer);
        }

        count++;

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file")) {
                IM_ASSERT(payload->DataSize == sizeof(AssetPayload));
                const AssetPayload* data = static_cast<const AssetPayload*>(payload->Data);

                _args = data->GUID;    
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityPayload")) {
                IM_ASSERT(payload->DataSize == sizeof(EntityPayload));

                _args = static_cast<const EntityPayload*>(payload->Data)->guid;
            }

            ImGui::EndDragDropTarget();
        }
    }

    template <typename U>
    void operator()(std::vector<U>& _args) {

        if (ImGui::TreeNodeEx(m_Array[count].c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

            if constexpr (std::is_class_v<U> && requires { U::Names(); }) {
                int _count{};
                for (U& x : _args) {
                    ImGui::PushID(_count++);
                    x.ApplyFunction(DrawComponents<decltype(x.Names())>{x.Names()});
                    ImGui::PopID();
                }
            }
            else {
                int _count{};
                for (U& x : _args) {
                    ImGui::PushID(_count++);
                    (*this)(x); // Handle non-class types
                    count--;// minus so no subsciprt error
                    ImGui::PopID();
                }
            }




            static std::string buttonLabel = "Add " + m_Array[count];
            static std::string rmvButtonLabel = "Remove " + m_Array[count];
            if (ImGui::Button(buttonLabel.c_str())) {
                _args.push_back(U{}); // add a default-constructed element
            }

            


            if (!_args.empty() ) {
                ImGui::SameLine();
                if (ImGui::Button(rmvButtonLabel.c_str())) {
                    _args.pop_back(); // add a default-constructed element
                }

               
            }

            ImGui::TreePop();
        }


        count++;

    }

    template <typename K>
    void operator()(K& _args) {
        if constexpr (std::is_class_v<K>) {

            if (ImGui::TreeNodeEx(m_Array[count].c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                _args.ApplyFunction(DrawComponents<decltype(_args.Names())>{_args.Names()});
                ImGui::TreePop();
            }
            
            
        }
        count++;
    }

};

class IEditorActionInvoker {
public:
    virtual ~IEditorActionInvoker() = default;

    // Pure virtual function to draw the component's UI in ImGui or other editors
    virtual void Draw(void* componentData) = 0;

    virtual void AddComponent(ecs::EntityID ID) = 0;
};

template <typename T>
class EditorActionInvoker : public IEditorActionInvoker {
public:
    void Draw(void* componentData) override {
        T* component = static_cast<T*>(componentData);
        ecs::EntityID ID = component->entity;

        bool open = ImGui::CollapsingHeader(component->classname(), ImGuiTreeNodeFlags_DefaultOpen);

        if (ImGui::BeginPopupContextItem()) {
            if (T::classname() != ecs::TransformComponent::classname() && ImGui::MenuItem("Delete Component")) {
                
                ecs::ECS::GetInstance()->RemoveComponent<T>(ID);
            }
            if (ImGui::MenuItem("Reset Component")) {
                ecs::ECS::GetInstance()->ResetComponent<T>(ID);


            }

            ImGui::EndPopup();
        }

        if (open) {
            component->ApplyFunction(DrawComponents(component->Names()));
        }
        
    }

    void AddComponent(ecs::EntityID ID) override {

        ecs::ECS::GetInstance()->AddComponent<T>(ID);
    }
};
