#include "decryption_file_scene.h"
// #include <gui/modules/file_browser.h>
// #include <dialogs/dialogs_module_file_browser.h>
#include <dialogs/dialogs.h>
#include "../keyhold.h"
#include <storage/storage.h>
// #include <keyhold.pb.h>
#include <pb_decode.h>
// #include <toolbox/stream/stream.h>
// #include <toolbox/stream/file_stream.h>

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
        File* file = storage_file_alloc(app->storage);
        if(!storage_file_open(
               file, furi_string_get_cstr(app->file_browser_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            storage_file_free(file);
            scene_manager_search_and_switch_to_another_scene(
                app->scene_manager, KeyholdSceneMainMenu);
        }
        // 2 conditionals because we need the size after opening
        uint64_t file_size = storage_file_size(file);
        FURI_LOG_I("keyhold", "%lld", file_size);
        uint8_t* ptr = encryptor_config_encrypted_bytes_buf_init(app->encryptor_config, file_size);
        storage_file_read(file, ptr,
                          file_size); //  I hope this works

        storage_file_close(file);
        storage_file_free(file);
        FURI_LOG_I("keyhold", "%s", ptr);
        scene_manager_next_scene(app->scene_manager, KeyholdSceneDecryptionConfig);
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
