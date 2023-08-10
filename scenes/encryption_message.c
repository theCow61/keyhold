#include "encryptionscreen_scene.h"
#include "../keyhold.h"
#include "../lib/monocypher/monocypher.h"

// SIGNED TO UNSIGNED CAST

void keyhold_callback_encrypt(void* ctx) {
    App* app = ctx;
    encryptor_config_sync_pbuffer_str(app->encryptor_config, app->all_purpose_string);
    FURI_LOG_D("keyhold", "%s", encryptor_config_get_plain_buffer(app->encryptor_config));

    // wipe all_purpose_string
    crypto_wipe(app->all_purpose_string, 255);
    app->all_purpose_string[0] = '\0'; // avoid showing wierd stuff maybe

    scene_manager_handle_custom_event(app->scene_manager, 0);
}

void keyhold_scene_on_enter_encryptionmessage(void* ctx) {
    App* app = ctx;

    app->all_purpose_string[0] =
        '\0'; // generate random identity name input should do this but could mess things up

    text_input_reset(app->view_textinput);
    text_input_set_header_text(app->view_textinput, "Enter Message to be Encrypted:");

    text_input_set_result_callback(
        app->view_textinput, keyhold_callback_encrypt, app, app->all_purpose_string, 254, true);

    view_dispatcher_switch_to_view(app->vp, KeyholdViewTextInput);
}
bool keyhold_scene_on_event_encryptionmessage(void* ctx, SceneManagerEvent evt) {
    // take u to config variablelist screen
    UNUSED(evt);
    App* app = ctx;

    bool consumed = false;

    // Back event will end up executing without this condition
    if(evt.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, KeyholdSceneEncryptionConfig);
        consumed = true;
    }

    return consumed;
}
void keyhold_scene_on_exit_encryptionmessage(void* ctx) {
    App* app = ctx;
    text_input_reset(app->view_textinput);
}