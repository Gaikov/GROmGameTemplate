project(GameTemplate)

set(sources
        GameApp.cpp
        GameAppInfo.cpp
        TouchesContainer.cpp
        TouchesContainer.h
)

if (ANDROID)
    add_library(${GAME_MODULE_NAME} STATIC ${sources})
    target_link_libraries(${GAME_MODULE_NAME} ${GROM_ENGINE_LIBS})
elseif (WEB_ASM)
    add_executable(${GAME_MODULE_NAME} ${sources})
    set_target_properties(${GAME_MODULE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shell_minimal.html"
    )

    target_link_options(${GAME_MODULE_NAME} PRIVATE
            -O3
            "-sEXPORTED_FUNCTIONS=['_nsEngine_OnActivateApp','_nsEngine_OnKeyDown','_nsEngine_OnKeyUp','_nsEngine_OnCharDown','_main']"
            "-sEXPORTED_RUNTIME_METHODS=['cwrap']"
            "-sINCOMING_MODULE_JS_API=ENVIRONMENT,arguments,canvas,dynamicLibraries,elementPointerLock,instantiateWasm,locateFile,monitorRunDependencies,noExitRuntime,noInitialRun,onAbort,onExit,onRuntimeInitialized,postRun,preInit,preRun,print,printErr,setStatus,statusMessage,stderr,stdin,stdout,thisProgram,wasm,websocket,preMainLoop"
            -sALLOW_MEMORY_GROWTH=1
            -sSTACK_SIZE=500000
            -sUSE_GLFW=3
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell_minimal.html"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/"
            "SHELL:--exclude-file *.dylib"
            "SHELL:--exclude-file *.log"
            "SHELL:--exclude-file */metal/*"
    )

    target_link_libraries(${GAME_MODULE_NAME} ${GROM_ENGINE_LIBS})
else()
    add_executable(${GAME_MODULE_NAME} ${sources})
    target_link_libraries(${GAME_MODULE_NAME} ${GROM_ENGINE_LIBS})

    if (APPLE)
        add_custom_command(TARGET ${GAME_MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/assets/libEGL.dylib"
                "$<TARGET_FILE_DIR:${GAME_MODULE_NAME}>/libEGL.dylib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/assets/libGLESv2.dylib"
                "$<TARGET_FILE_DIR:${GAME_MODULE_NAME}>/libGLESv2.dylib"
        )
    endif ()
endif ()

include_directories(./)
