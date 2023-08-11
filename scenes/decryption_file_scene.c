#include "decryption_file_scene.h"
// #include <gui/modules/file_browser.h>
#include <dialogs/dialogs_module_file_browser.h>
#include <dialogs/dialogs.h>
#include "../keyhold.h"

static bool keyhold_file_select(App* app) {
    furi_assert(app);

    // Input events and views are managed by file_browser
    bool res = dialog_file_browser_show(
        app->dialogs, app->file_browser_path, app->file_browser_path, NULL);

    return res;
}

void keyhold_scene_on_enter_decryptionfile(void* ctx) {
    App* app = ctx;
    if(keyhold_file_select(app)) {
        // go to decryption_config_scene pick keys like encryption_config_scene
        scene_manager_next_scene(app->scene_manager, KeyholdSceneMainMenu);
    }
}

bool keyhold_scene_on_event_decryptionfile(void* ctx, SceneManagerEvent evt) {
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}

void keyhold_scene_on_exit_decryptionfile(void* ctx) {
    UNUSED(ctx);
}