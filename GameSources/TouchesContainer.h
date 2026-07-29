// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <map>

#include "Engine/display/container/VisualContainer2d.h"

class nsTouchesContainer final : public nsVisualContainer2d {
public:
    void Destroy() override;

protected:
    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerMove(float x, float y, int pointerId) override;
    bool OnPointerUp(float x, float y, int pointerId) override;
    void OnPointerCancel(int pointerId) override;

private:
    nsVisualContainer2d *CreateTouch(int pointerId);
    void UpdateTouch(int pointerId, float x, float y);
    void RemoveTouch(int pointerId);

private:
    std::map<int, nsVisualContainer2d *> _touches;
};
