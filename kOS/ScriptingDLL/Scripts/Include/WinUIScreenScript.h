#pragma once
#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"

class WinSceneScript : public TemplateSC {
public:
    utility::GUID winTextObject;
    utility::GUID currentScoreTextObject;
    utility::GUID highScoreTextObject;
    utility::GUID restartTextObject;

    int currentScore = 100;
    int highScore = 500;
    bool showScreen = false;

private:
    // Cache EntityIDs
    ecs::EntityID winTextID = 0;
    ecs::EntityID currentScoreTextID = 0;
    ecs::EntityID highScoreTextID = 0;
    ecs::EntityID restartTextID = 0;
    bool wasShowScreen = false;  // Track previous state

public:
    void Start() override {
        // Get EntityIDs once and cache them
        winTextID = ecsPtr->GetEntityIDFromGUID(winTextObject);
        currentScoreTextID = ecsPtr->GetEntityIDFromGUID(currentScoreTextObject);
        highScoreTextID = ecsPtr->GetEntityIDFromGUID(highScoreTextObject);
        restartTextID = ecsPtr->GetEntityIDFromGUID(restartTextObject);

        // Hide everything at start
        HideAllElements();
    }

    void Update() override {
        if (showScreen) {
            // Update text content
            if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(winTextID)) {
                tc->text = "YOU WIN!";
            }
            if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(currentScoreTextID)) {
                tc->text = "Current Score: " + std::to_string(currentScore);
            }
            if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(highScoreTextID)) {
                tc->text = "High Score: " + std::to_string(highScore);
            }
            if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(restartTextID)) {
                tc->text = "Press L to Restart";
            }

            // Show everything
            ShowAllElements();

            // Check for restart
            if (Input->IsKeyTriggered(keys::L)) {
                // TODO: Restart logic
            }
        }
        else if (wasShowScreen) {
            // Only hide when transitioning from shown to hidden
            HideAllElements();
        }

        wasShowScreen = showScreen;
    }

private:
    void ShowAllElements() {
        ecsPtr->SetActive(winTextID, true);
        ecsPtr->SetActive(currentScoreTextID, true);
        ecsPtr->SetActive(highScoreTextID, true);
        ecsPtr->SetActive(restartTextID, true);
    }

    void HideAllElements() {
        ecsPtr->SetActive(winTextID, false);
        ecsPtr->SetActive(currentScoreTextID, false);
        ecsPtr->SetActive(highScoreTextID, false);
        ecsPtr->SetActive(restartTextID, false);
    }

public:
    REFLECTABLE(WinSceneScript, winTextObject, currentScoreTextObject,
        highScoreTextObject, restartTextObject, currentScore, highScore, showScreen);
};
