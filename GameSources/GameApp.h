// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GameApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/container/VisualContainer2d.h"
#include "Engine/GameApp.h"
#include "Engine/RenDevice.h"

class nsGameTemplate : public IGameApp {
    nsVisualContainer2d *_stage = nullptr;
    IRenDevice *_device = nullptr;
    IVertexBuffer *_testTri = nullptr;

    bool Init() override;
    void Release() override;
    void Loop(float frameTime) override;
    void DrawWorld() override;

    void OnActivate(bool active) override;
    void OnPause(bool paused) override;

    bool InitDialog() override;
    int GetWindowIcon() override;
    IUserInput *GetUserInput() override;
    void GetGUIDimension(int &width, int &height) override;
    const char *GetVersionInfo() override;
};