#include "decryption_file_scene.h"
#include <gui/modules/file_browser.h>
#include "../keyhold.h"

void keyhold_scene_on_enter_decryptionfile(void* ctx) {
    App* app = ctx;

    file_browser_stop(app->view_filebrowser);
    // file_browser_start(app->view_filebrowser);
}

bool keyhold_scene_on_event_decryptionfile(void* ctx, SceneManagerEvent evt) {
}

void keyhold_scene_on_exit_decryptionfile(void* ctx) {
    App* app = ctx;
    file_browser_stop(app->view_filebrowser);
}