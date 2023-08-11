#include "file_saver_scene.h"

#include "../keyhold.h"
#include <gui/modules/file_browser.h>

void keyhold_scene_on_enter_filesaver(void* ctx) {
    UNUSED(ctx);
    // App* app = ctx;

    // file_browser_stop(app->view_filebrowser);
    // FuriString* filepath = furi_string_alloc();
    // file_browser_start(app->view_filebrowser, filepath);
}

bool keyhold_scene_on_event_filesaver(void* ctx, SceneManagerEvent evt) {
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}

void keyhold_scene_on_exit_filesaver(void* ctx) {
    UNUSED(ctx);
}