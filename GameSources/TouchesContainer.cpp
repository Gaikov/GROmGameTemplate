// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.

#include "TouchesContainer.h"

#include "Engine/display/factory/VisualFactory2d.h"
#include "Engine/display/text/TextLabel.h"
#include "nsLib/log.h"

namespace {
constexpr const char *TOUCH_LAYOUT = "tests/layouts/touch.layout";
}

void nsTouchesContainer::Destroy() {
    _touches.clear();
    nsVisualContainer2d::Destroy();
}

bool nsTouchesContainer::OnPointerDown(float x, float y, int pointerId) {
    if (_touches.find(pointerId) == _touches.end()) {
        if (auto touch = CreateTouch(pointerId)) {
            _touches[pointerId] = touch;
            AddChild(touch);
        }
    }

    UpdateTouch(pointerId, x, y);
    return false;
}

bool nsTouchesContainer::OnPointerMove(float x, float y, int pointerId) {
    UpdateTouch(pointerId, x, y);
    return false;
}

bool nsTouchesContainer::OnPointerUp(float x, float y, int pointerId) {
    UpdateTouch(pointerId, x, y);
    RemoveTouch(pointerId);
    return false;
}

void nsTouchesContainer::OnPointerCancel(int pointerId) {
    RemoveTouch(pointerId);
}

nsVisualContainer2d *nsTouchesContainer::CreateTouch(int pointerId) {
    auto object = nsVisualFactory2d::Shared()->Create(TOUCH_LAYOUT);
    auto touch = dynamic_cast<nsVisualContainer2d *>(object);
    if (!touch) {
        Log::Warning("Touch layout root must be a container: %s", TOUCH_LAYOUT);
        if (object) {
            object->Destroy();
        }
        return nullptr;
    }

    auto label = dynamic_cast<nsTextLabel *>(touch->GetChildByIdRecursive("label"));
    if (!label) {
        Log::Warning("Touch layout label not found: %s", TOUCH_LAYOUT);
        touch->Destroy();
        return nullptr;
    }

    label->SetInt(pointerId);
    return touch;
}

void nsTouchesContainer::UpdateTouch(int pointerId, float x, float y) {
    if (const auto it = _touches.find(pointerId); it != _touches.end()) {
        it->second->origin.pos = {x, y};
    }
}

void nsTouchesContainer::RemoveTouch(int pointerId) {
    const auto it = _touches.find(pointerId);
    if (it == _touches.end()) {
        return;
    }

    auto touch = it->second;
    _touches.erase(it);
    touch->Destroy();
}
