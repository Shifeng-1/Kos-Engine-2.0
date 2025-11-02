#pragma once

#include "Config/pch.h"

struct EntityPayload {
    ecs::EntityID id;
    utility::GUID guid;
};

struct AssetPayload {
    char path[260];
    utility::GUID GUID;
    REFLECTABLE(AssetPayload, path, GUID);
};