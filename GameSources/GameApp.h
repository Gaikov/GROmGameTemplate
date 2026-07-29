// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GameApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/BaseGame2DApp.h"

class nsTouchesContainer;

class nsGameTemplate : public nsBaseGame2DApp {
    nsVisualContainer2d *_content = nullptr;
    nsTouchesContainer *_touches = nullptr;

    bool Init() override;
    void Release() override;
    void Loop(float frameTime) override;
    void DrawWorld() override;

    void OnActivate(bool active) override;
    void OnPause(bool paused) override;

    bool InitDialog() override;
    int GetWindowIcon() override;
    const char *GetVersionInfo() override;
};
