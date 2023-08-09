#include "encryptionscreen_scene.h"
#include "../keyhold.h"
#include "../lib/monocypher/monocypher.h"

void keyhold_callback_encrypt(void* ctx) {
    App* app = ctx;
    encryptor_config_sync_pbuffer_str(app->encryptor_config, app->all_purpose_string);
    FURI_LOG_D("keyhold", "%s", encryptor_config_get_plain_buffer(app->encryptor_config));

    // wipe all_purpose_string
    crypto_wipe(app->all_purpose_string, 255);
    app->all_purpose_string[0] = '\0'; // avoid showing wierd stuff maybe
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
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}
void keyhold_scene_on_exit_encryptionmessage(void* ctx) {
    App* app = ctx;
    text_input_reset(app->view_textinput);
}