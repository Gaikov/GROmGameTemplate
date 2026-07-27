#include <jni.h>

#include "Platform/AndroidPlatform/AndroidLogPolicy.h"
#include "Renderer.h"
#include "Engine/Platform.h"
#include "Platform/AndroidPlatform/AndroidPlatform.h"
#include "Engine/engine.h"
#include "Core/Config.h"
#include "AndroidSoftInput.h"

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

extern "C" {

#include <game-activity/native_app_glue/android_native_app_glue.c>

/*!
 * Handles commands sent to this Android application
 * @param pApp the app the commands are coming from
 * @param cmd the command to handle
 */
static bool g_started = false;

void handle_cmd(android_app *pApp, int32_t cmd) {
    AndroidPlatform *platform = dynamic_cast<AndroidPlatform *>(App_GetPlatform());
    JavaObject::sp_t activity;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            // A new window is created, associate a renderer with it. You may replace this with a
            // "game" class if that suits your needs. Remember to change all instances of userData
            // if you change the class here as a reinterpret_cast is dangerous this in the
            // android_main function and the APP_CMD_TERM_WINDOW handler case.
            auto renderer = new Renderer(pApp);
            if (!renderer->IsInitialized()) {
                delete renderer;
                break;
            }
            pApp->userData = renderer;

            if (!g_started) {
                g_started = true;
                nsEngine::Init();
            } else {
                nsEngine::OnActivateApp(true);
            }

            activity = new JavaObject(pApp->activity->javaGameActivity, pApp->activity->env, pApp->activity->vm);
            platform->_activity = activity;

            break;
        }
        case APP_CMD_TERM_WINDOW:
            // The window is being destroyed. Use this to clean up your userData to avoid leaking
            // resources.
            //
            // We have to check if userData is assigned just in case this comes in really quickly
            platform->_activity = nullptr;

            nsEngine::OnActivateApp(false);
            if (pApp->userData) {
                auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);
                pApp->userData = nullptr;
                delete pRenderer;
            }

            break;
        default:
            break;
    }
}

/*!
 * Enable the motion events you want to handle; not handled events are
 * passed back to OS for further processing. For this example case,
 * only pointer and joystick devices are enabled.
 *
 * @param motionEvent the newly arrived GameActivityMotionEvent.
 * @return true if the event is from a pointer or joystick device,
 *         false for all other input devices.
 */
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

/*!
 * This the main entry point for a native activity
 */

static AndroidLogPolicy g_logPolicy;

void android_main(struct android_app *pApp) {

    if (!g_started) {
        Log::Init();
        Log::Shared()->AddPolicy(&g_logPolicy);
        AndroidPlatform::Create(pApp->activity->env,
                                pApp->activity->vm,
                                pApp->activity->assetManager,
                                pApp->activity->internalDataPath);
    }

    // Register an event handler for Android events
    pApp->onAppCmd = handle_cmd;

    // Set input event filters (set it to NULL if the app wants to process all inputs).
    // Note that for key inputs, this example uses the default default_key_filter()
    // implemented in android_native_app_glue.c.
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    int events;
    android_poll_source *pSource;
    do {
        // Process all pending events before running game logic.
        while (ALooper_pollOnce(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }

        // Check if any user data is associated. This is assigned in handle_cmd
        if (pApp->userData) {

            // We know that our user data is a Renderer, so reinterpret cast it. If you change your
            // user data remember to change it here
            auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);

            // Process game input
            pRenderer->handleInput();

            // Render a frame
            pRenderer->render();
        }
    } while (!pApp->destroyRequested);
}
}
