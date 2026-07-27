#ifndef ANDROIDGLINVESTIGATIONS_RENDERER_H
#define ANDROIDGLINVESTIGATIONS_RENDERER_H

#include <memory>
#include "Platform/AndroidPlatform/AndroidPlatform.h"
#include "Platform/AndroidPlatform/GLNativeContext.h"

struct android_app;

class Renderer {
public:
    /*!
     * @param pApp the android_app this Renderer belongs to, needed to configure GL
     */
    Renderer(android_app *pApp);
    virtual ~Renderer();

    bool IsInitialized() const { return _initialized; }

    /*!
     * Handles input from the android_app.
     *
     * Note: this will clear the input queue
     */
    void handleInput();

    /*!
     * Renders all the models in the renderer
     */
    void render();

private:
    /*!
     * Performs necessary OpenGL initialization. Customize this if you want to change your EGL
     * context or application-wide settings.
     */
    bool initRenderer();

    /*!
     * Creates the models for this sample. You'd likely load a scene configuration from a file or
     * use some other setup logic in your full game.
     */

    GLNativeContext _context;
    android_app *app_;

    AndroidPlatform *_platform;
    bool _initialized = false;
};

#endif //ANDROIDGLINVESTIGATIONS_RENDERER_H
