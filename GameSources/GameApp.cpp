//
// Created by Roman on 10/10/2024.
//

#include "GameApp.h"
#include "GameVersion.h"
#include "Engine/SndManager.h"
#include "Engine/Platform.h"
#include "Engine/Input.h"
#include "Engine/display/sprite/Sprite.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/display/particles/VisualParticles.h"
#include "Engine/renderer/font/FontsCache.h"
#include "Engine/renderer/particles/factory/ParticlesManager.h"
#include "TouchesContainer.h"

bool nsGameTemplate::Init() {
    if (!nsBaseGame2DApp::Init()) {
        return false;
    }

    auto device = GetRenderDevice();

    _content = new nsVisualContainer2d();
    _touches = new nsTouchesContainer();
    GetStage()->AddChild(_content);
    GetStage()->AddChild(_touches);

    auto renState = device->StateLoad("default/rs/gui_clamp.ggrs");

    auto back = device->TextureLoad("grom-logo.png");
    auto sprite = new nsSprite();
    sprite->renState = renState;
    sprite->desc.tex = back;
    sprite->desc.ResetSize().ComputeCenter();
    sprite->desc.color = nsColor::white;

    auto label = new nsTextLabel();
    label->color = nsColor::yellow;
    label->text = "Hello from GROm!";
    label->renState = renState;
    label->font = nsFontsCache::Shared()->LoadFont("tests/fonts/bmfont.fnt");

    _content->AddChild(sprite);
    _content->AddChild(label);

    int w, h;
    back->GetSize(w, h);
    nsVec2 corners[4] = {
        nsVec2(-w / 2.0f, -h / 2.0f),
        nsVec2(w / 2.0f, -h / 2.0f),
        nsVec2(w / 2.0f, h / 2.0f),
        nsVec2(-w / 2.0f, h / 2.0f)
    };

    auto particles = nsParticlesManager::Shared()->LoadParticles("tests/particles/point.txt");

    for (auto pos : corners) {
        auto parts = new nsVisualParticles();
        parts->origin.pos = pos;
        parts->GetSystem().behaviour = particles;
        parts->space = nsVisualParticles::GLOBAL;
        _content->AddChild(parts);
    }

    nsRect  r;
    label->GetBounds(r, _content);
    label->origin.pos = nsVec2(r.width, r.height + w / 1.5f) / -2;

    g_inp.ShowCursor(true);

    auto snd = nsSoundDevice::Shared()->Device();
    snd->TrackPlay(snd->TrackLoad("tests/test.ogg"));

    Log::Info("Device Env: %s", App_GetPlatform()->IsMobile() ? "Mobile" : "Desktop");

    return true;
}

void nsGameTemplate::Release() {
    _content = nullptr;
    _touches = nullptr;
    nsBaseGame2DApp::Release();
}

void nsGameTemplate::DrawWorld() {
    auto device = GetRenderDevice();
    device->ClearScene(CLR_CBUFF | CLR_ZBUFF | CLR_STENCIL);

    int width;
    int height;
    GetGUIDimension(width, height);
    _content->origin.pos = nsVec2(static_cast<float>(width), static_cast<float>(height)) / 2;

    nsBaseGame2DApp::DrawWorld();
}

void nsGameTemplate::Loop(float frameTime) {
    nsBaseGame2DApp::Loop(frameTime);
    _content->origin.angle = _content->origin.angle + frameTime;
}

void nsGameTemplate::OnActivate(bool active) {

}

void nsGameTemplate::OnPause(bool paused) {

}

int nsGameTemplate::GetWindowIcon() {
    return 0;
}

bool nsGameTemplate::InitDialog() {
    return true;
}

const char *nsGameTemplate::GetVersionInfo() {
    return GameVersion::Display;
}

static nsGameTemplate g_game;

IGameApp*	App_GetGame() {
    return &g_game;
}
