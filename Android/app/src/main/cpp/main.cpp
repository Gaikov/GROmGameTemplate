#include <jni.h>

#include "Platform/AndroidPlatform/AndroidLogPolicy.h"
#include "Renderer.h"
#include "Engine/Platform.h"
#include "Platform/AndroidPlatform/AndroidPlatform.h"
#include "Engine/engine.h"
#include "nsLib/log.h"

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <swappy/swappyGL.h>

#include <memory>

namespace {
class ScopedJniEnv {
public:
    explicit ScopedJniEnv(JavaVM *vm) : _vm(vm) {
        const jint status = _vm->GetEnv(
                reinterpret_cast<void **>(&_env), JNI_VERSION_1_6);
        if (status == JNI_EDETACHED &&
            _vm->AttachCurrentThread(&_env, nullptr) == JNI_OK) {
            _attached = true;
        } else if (status != JNI_OK) {
            _env = nullptr;
        }
    }

    ~ScopedJniEnv() {
        if (_attached) {
            _vm->DetachCurrentThread();
        }
    }

    JNIEnv *Get() const { return _env; }

private:
    JavaVM *_vm;
    JNIEnv *_env = nullptr;
    bool _attached = false;
};

class AndroidRuntime {
public:
    explicit AndroidRuntime(android_app *app)
            : _app(app),
              _platform(dynamic_cast<AndroidPlatform *>(App_GetPlatform())) {
        _platform->_activity = new JavaObject(
                _app->activity->javaGameActivity, _app->activity->vm);
    }

    ~AndroidRuntime() {
        Shutdown();
    }

    void HandleCommand(int32_t cmd) {
        switch (cmd) {
            case APP_CMD_INIT_WINDOW:
                CreateRenderer();
                break;

            case APP_CMD_TERM_WINDOW:
                DestroyRenderer();
                break;

            case APP_CMD_RESUME:
                _resumed = true;
                UpdateActivation();
                break;

            case APP_CMD_PAUSE:
            case APP_CMD_STOP:
                _resumed = false;
                UpdateActivation();
                break;

            case APP_CMD_GAINED_FOCUS:
                _focused = true;
                UpdateActivation();
                break;

            case APP_CMD_LOST_FOCUS:
                _focused = false;
                UpdateActivation();
                break;

            case APP_CMD_CONFIG_CHANGED:
            case APP_CMD_WINDOW_RESIZED:
            case APP_CMD_CONTENT_RECT_CHANGED:
                if (_renderer) {
                    _renderer->refreshSurfaceSize();
                }
                break;

            default:
                break;
        }
    }

    bool CanRender() const {
        return _active && _renderer && _renderer->IsInitialized();
    }

    void RenderFrame() {
        if (!CanRender()) {
            return;
        }
        _renderer->handleInput();
        if (!_renderer->render()) {
            Log::Warning("Recreating the Android EGL renderer");
            DestroyRenderer();
            CreateRenderer();
        }
    }

    void Shutdown() {
        if (_shutdown) {
            return;
        }
        _shutdown = true;

        SetActive(false);
        if (_engineInitialized) {
            nsEngine::Release(false);
            _engineInitialized = false;
        }
        _renderer.reset();
    }

private:
    void CreateRenderer() {
        if (!_app->window || _renderer) {
            return;
        }

        auto renderer = std::make_unique<Renderer>(_app);
        if (!renderer->IsInitialized()) {
            return;
        }
        _renderer = std::move(renderer);

        if (!_engineInitialized) {
            if (!nsEngine::Init()) {
                Log::Error("Failed to initialize GROm Engine");
                _renderer.reset();
                return;
            }
            _engineInitialized = true;
            _active = _resumed && _focused;
        } else {
            UpdateActivation();
        }
    }

    void DestroyRenderer() {
        SetActive(false);
        _renderer.reset();
    }

    void UpdateActivation() {
        SetActive(_engineInitialized && _renderer && _resumed && _focused);
    }

    void SetActive(bool active) {
        if (!_engineInitialized || _active == active) {
            return;
        }
        _active = active;
        nsEngine::OnActivateApp(active);
    }

private:
    android_app *_app;
    AndroidPlatform *_platform;
    std::unique_ptr<Renderer> _renderer;
    bool _engineInitialized = false;
    bool _resumed = false;
    bool _focused = false;
    bool _active = false;
    bool _shutdown = false;
};

void HandleCommand(android_app *app, int32_t cmd) {
    auto *runtime = static_cast<AndroidRuntime *>(app->userData);
    if (runtime) {
        runtime->HandleCommand(cmd);
    }
}

bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

static AndroidLogPolicy g_logPolicy;
}

extern "C" {
void android_main(struct android_app *pApp) {
    Log::Init();
    Log::Shared()->AddPolicy(&g_logPolicy);
    ScopedJniEnv jniEnv(pApp->activity->vm);
    if (!jniEnv.Get()) {
        Log::Error("Failed to attach the Android render thread to the JVM");
        Log::Release();
        return;
    }

    AndroidPlatform::Create(jniEnv.Get(),
                            pApp->activity->vm,
                            pApp->activity->assetManager,
                            pApp->activity->internalDataPath);

    if (!SwappyGL_init(jniEnv.Get(), pApp->activity->javaGameActivity)) {
        Log::Warning("Swappy failed to initialize; using EGL buffer swaps");
    }

    // Register an event handler for Android events
    AndroidRuntime runtime(pApp);
    pApp->userData = &runtime;
    pApp->onAppCmd = HandleCommand;

    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    int events;
    android_poll_source *pSource;
    while (!pApp->destroyRequested) {
        const int timeout = runtime.CanRender() ? 0 : -1;
        while (ALooper_pollOnce(timeout, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
            if (pApp->destroyRequested) {
                break;
            }
            if (runtime.CanRender()) {
                break;
            }
        }

        if (!pApp->destroyRequested) {
            runtime.RenderFrame();
        }
    }

    runtime.Shutdown();
    pApp->userData = nullptr;
    SwappyGL_destroy();
    AndroidPlatform::Destroy();
    Log::Release();
}
}
