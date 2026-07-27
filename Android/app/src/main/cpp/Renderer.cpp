#include "Renderer.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <memory>
#include <vector>

#include "Platform/AndroidPlatform/AndroidLogPolicy.h"
#include "Engine/engine.h"
#include "Platform/AndroidPlatform/AndroidPlatform.h"

namespace {
int GetEngineModifiers(int32_t metaState) {
    int modifiers = 0;
    if (metaState & AMETA_SHIFT_ON) modifiers |= 0x0001;
    if (metaState & AMETA_CTRL_ON) modifiers |= 0x0002;
    if (metaState & AMETA_ALT_ON) modifiers |= 0x0004;
    if (metaState & AMETA_META_ON) modifiers |= 0x0008;
    if (metaState & AMETA_CAPS_LOCK_ON) modifiers |= 0x0010;
    if (metaState & AMETA_NUM_LOCK_ON) modifiers |= 0x0020;
    return modifiers;
}
}

Renderer::Renderer(android_app *pApp) :
        app_(pApp),
        _context(pApp->window) {
    _platform = dynamic_cast<AndroidPlatform *>(App_GetPlatform());
    _initialized = initRenderer();
}

Renderer::~Renderer() {
    _platform->SetGLContext(nullptr);
    _context.Release();
}

bool Renderer::initRenderer() {
    if (!_context.Init()) {
        Log::Error("Failed to initialize the OpenGL ES 3 renderer");
        return false;
    }
    _platform->SetGLContext(&_context);
    _platform->SetScreenSize(_context.GetSurfaceWidth(), _context.GetSurfaceHeight());
    return true;
}

bool Renderer::render() {
    if (!_initialized) {
        return false;
    }
    refreshSurfaceSize();
    if (_context.NeedsRecreation()) {
        return false;
    }
    _context.BeginRender();
    nsEngine::MainLoop();
    return !_context.NeedsRecreation();
}

void Renderer::refreshSurfaceSize() {
    const EGLint width = _context.GetSurfaceWidth();
    const EGLint height = _context.GetSurfaceHeight();
    if (width > 0 && height > 0) {
        _platform->SetScreenSize(width, height);
    }
}

void Renderer::handleInput() {
    // handle all queued inputs
    auto *inputBuffer = android_app_swap_input_buffers(app_);
    if (!inputBuffer) {
        // no inputs yet.
        return;
    }

    // handle motion events (motionEventsCounts can be 0).
    for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
        auto &motionEvent = inputBuffer->motionEvents[i];
        auto action = motionEvent.action;

        const int actionMasked = action & AMOTION_EVENT_ACTION_MASK;
        const uint32_t pointerIndex =
                (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        switch (actionMasked) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN: {
                if (pointerIndex >= motionEvent.pointerCount) {
                    Log::Warning("Invalid pointer down index: %u/%u",
                                 pointerIndex, motionEvent.pointerCount);
                    break;
                }
                const auto &pointer = motionEvent.pointers[pointerIndex];
                const float x = GameActivityPointerAxes_getX(&pointer);
                const float y = GameActivityPointerAxes_getY(&pointer);
                _activePointers[pointer.id] = {x, y};
                nsEngine::OnPointerDown(pointer.id, x, y);
                break;
            }

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP: {
                if (pointerIndex >= motionEvent.pointerCount) {
                    Log::Warning("Invalid pointer up index: %u/%u",
                                 pointerIndex, motionEvent.pointerCount);
                    break;
                }
                const auto &pointer = motionEvent.pointers[pointerIndex];
                const float x = GameActivityPointerAxes_getX(&pointer);
                const float y = GameActivityPointerAxes_getY(&pointer);
                nsEngine::OnPointerUp(pointer.id, x, y);
                _activePointers.erase(pointer.id);
                break;
            }

            case AMOTION_EVENT_ACTION_MOVE: {
                for (uint32_t index = 0; index < motionEvent.pointerCount; index++) {
                    const auto &pointer = motionEvent.pointers[index];
                    const float x = GameActivityPointerAxes_getX(&pointer);
                    const float y = GameActivityPointerAxes_getY(&pointer);
                    _activePointers[pointer.id] = {x, y};
                    nsEngine::OnPointerMove(pointer.id, x, y);
                }
                break;
            }

            case AMOTION_EVENT_ACTION_CANCEL:
                for (const auto &[pointerId, position] : _activePointers) {
                    nsEngine::OnPointerUp(pointerId, position.first, position.second);
                }
                _activePointers.clear();
                break;

            default:
                break;
        }
    }
    // clear the motion input count in this buffer for main thread to re-use.
    android_app_clear_motion_events(inputBuffer);

    // handle input key events.
    for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
        auto &keyEvent = inputBuffer->keyEvents[i];
        const int modifiers = GetEngineModifiers(keyEvent.metaState);
        //Log::Info("Key: %i ", keyEvent.keyCode);
        switch (keyEvent.action) {
            case AKEY_EVENT_ACTION_DOWN:
                _platform->GetKeyboard()->SetKeyPressed(keyEvent.keyCode, true);
                nsEngine::OnKeyDown(keyEvent.keyCode, keyEvent.repeatCount != 0, modifiers);
                if (keyEvent.unicodeChar && keyEvent.unicodeChar < 256) {
                    nsEngine::OnCharDown(keyEvent.unicodeChar);
                }
                break;
            case AKEY_EVENT_ACTION_UP:
                _platform->GetKeyboard()->SetKeyPressed(keyEvent.keyCode, false);
                nsEngine::OnKeyUp(keyEvent.keyCode, modifiers);
                break;
            case AKEY_EVENT_ACTION_MULTIPLE:
                // Deprecated since Android API level 29.
                Log::Info("Multiple Key Actions");
                break;
            default:
                Log::Info("Unknown KeyEvent Action: %i", keyEvent.action);
        }
    }
    // clear the key input count too.
    android_app_clear_key_events(inputBuffer);
}
