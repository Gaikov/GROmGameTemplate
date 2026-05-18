//
// Created by Roman on 10/10/2024.
//

#include "GameApp.h"
#include "Engine/SndManager.h"
#include "Engine/Platform.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/RenManager.h"
#include "Engine/Input.h"
#include "Engine/TimeFormat.h"
#include "Engine/display/sprite/Sprite.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/utils/AppUtils.h"
#include "Engine/display/particles/VisualParticles.h"
#include "Engine/renderer/font/FontsCache.h"
#include "Engine/renderer/particles/factory/ParticlesManager.h"
#include "Engine/renderer/particles/ParticleSystem.h"

bool nsGameTemplate::Init() {
    _device = nsRenDevice::Shared()->Device();

    _testTri = _device->VerticesCreate(3, 3, false, true);
    auto verts = _testTri->GetWriteVertices();
    verts[0].v = nsVec3( 0.0f,  0.6f, 0);
    verts[1].v = nsVec3(-0.5f, -0.4f, 0);
    verts[2].v = nsVec3( 0.5f, -0.4f, 0);
    verts[0].c = 0xff0000ff;
    verts[1].c = 0xff00ff00;
    verts[2].c = 0xffff0000;
    auto indices = _testTri->GetWriteIndices();
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    _testTri->SetValidVertices(3);
    _testTri->SetValidIndices(3);

    _stage = new nsVisualContainer2d();
    auto renState = _device->StateLoad("default/rs/gui_clamp.ggrs");

    auto back = _device->TextureLoad("grom-logo.png");
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

    _stage->AddChild(sprite);
    _stage->AddChild(label);

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
        _stage->AddChild(parts);
    }

    nsRect  r;
    label->GetBounds(r, _stage);
    label->origin.pos = nsVec2(r.width, r.height + w / 1.5f) / -2;

    g_inp.ShowCursor(true);

    auto snd = nsSoundDevice::Shared()->Device();
    snd->TrackPlay(snd->TrackLoad("tests/test.ogg"));

    Log::Info("Device Env: %s", App_GetPlatform()->IsMobile() ? "Mobile" : "Desktop");

    return true;
}

void nsGameTemplate::Release() {
    if (_testTri) {
        _device->VerticesRelease(_testTri);
        _testTri = nullptr;
    }
    if (_stage) {
        _stage->Destroy();
    }
}

void nsGameTemplate::DrawWorld() {
    _device->ClearScene(CLR_CBUFF | CLR_ZBUFF | CLR_STENCIL);

    _stage->origin.pos = nsAppUtils::GetClientSize() / 2;

    nsVisualSceneRender2d::DrawScene(_stage);

    static float testAngle = 0;
    testAngle += 0.03f;
    nsMatrix proj, view, model;
    proj.Identity();
    view.Identity();
    model.Identity();
    model.RotateZ(testAngle);
    _device->LoadProjMatrix(proj);
    _device->LoadViewMartix(view);
    _device->LoadMatrix(model);
    _device->VerticesDraw(_testTri);
}

void nsGameTemplate::Loop(float frameTime) {
    _stage->Loop();
    _stage->origin.angle = _stage->origin.angle + frameTime;
}

IUserInput *nsGameTemplate::GetUserInput() {
    return _stage;
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


void nsGameTemplate::GetGUIDimension(int &width, int &height) {
    App_GetPlatform()->GetClientSize(width, height);
}

const char *nsGameTemplate::GetVersionInfo() {
    return "Template 1.0.0";
}

static nsGameTemplate g_game;

IGameApp*	App_GetGame() {
    return &g_game;
}

