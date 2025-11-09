#pragma once
#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"

class HealthBarScript : public TemplateSC {
public:
    int healthValue = 6;  // Set this value (1-6) to display the corresponding sprite

    // References to all 6 health bar sprites
    utility::GUID healthbar1Sprite;
    utility::GUID healthbar2Sprite;
    utility::GUID healthbar3Sprite;
    utility::GUID healthbar4Sprite;
    utility::GUID healthbar5Sprite;
    utility::GUID healthbar6Sprite;

    void Start() override {
        // Initialize the sprite on start
        UpdateHealthBarSprite();
    }

    void Update() override {
        // Update the sprite based on healthValue
        UpdateHealthBarSprite();
    }

private:
    void UpdateHealthBarSprite() {
        if (auto* sc = ecsPtr->GetComponent<ecs::SpriteComponent>(entity)) {
            if (healthValue >= 6) {
                sc->spriteGUID = healthbar6Sprite;
            }
            else if (healthValue >= 5) {
                sc->spriteGUID = healthbar5Sprite;
            }
            else if (healthValue >= 4) {
                sc->spriteGUID = healthbar4Sprite;
            }
            else if (healthValue >= 3) {
                sc->spriteGUID = healthbar3Sprite;
            }
            else if (healthValue >= 2) {
                sc->spriteGUID = healthbar2Sprite;
            }
            else {
                sc->spriteGUID = healthbar1Sprite;
            }
        }
    }

public:
    REFLECTABLE(HealthBarScript, healthValue,
        healthbar1Sprite, healthbar2Sprite, healthbar3Sprite,
        healthbar4Sprite, healthbar5Sprite, healthbar6Sprite);
};