#include "encryption_config_scene.h"
#include "../keyer.h"
#include "../saves.h"
#include "gui/view_dispatcher.h"
#include <gui/modules/variable_item_list.h>
#include <storage/storage.h> // temp

#include "../keyhold.h"

// I can either
// 1. render option as needed for user, not rending more than whats needed BUT a whole deref for every rendering
// 2. render everything at once regardless if user will ever see it, minimal derefs. Tho I would have to do a strdup for each value or else everything will refer to same thing.

void keyhold_callback_change_fromR(
    VariableItem* vi) { // we will render text as user filps through selection

    // Option 1
    App* app = variable_item_get_context(vi);

    uint8_t idx = variable_item_get_current_value_index(vi);
    variable_item_set_current_value_text(vi, saves_get_save_at(app->saves, idx));

    // This is needed as we can't retrieve our VariableItem in enter callback besides making a maybe memory leaking pointer holder
    app->selector_names.name1 = idx;
}

// We make 2 to avoid branching

void keyhold_callback_change_toR(
    VariableItem* vi) { // we will render text as user filps through selection

    // Option 1
    App* app = variable_item_get_context(vi);

    uint8_t idx = variable_item_get_current_value_index(vi);
    variable_item_set_current_value_text(vi, saves_get_save_at(app->saves, idx));

    app->selector_names.name2 = idx;
}

void keyhold_callback_dencrypt(void* ctx, uint32_t idx) {
    if(idx != 2)
        return; // 0 is when clicked selector 1 is when clicked selector 2 is when clicked our button

    App* app = ctx;

    size_t n_saves = saves_get_number_of_saves(app->saves);
    if(n_saves == 0) return; // no saves

    const char* from_save = saves_get_save_at(app->saves, app->selector_names.name1);
    const char* to_save = saves_get_save_at(app->saves, app->selector_names.name2);

    Identity from_idn = keyer_get_pub_identity(app->storage, from_save);
    // Identity to_idn = keyer_get_identity(app->storage, to_save);
    Identity to_idn = keyer_get_correct_identity(app->storage, app->encrypted_keys, to_save, app->selector_names.name2);

    // might have to modify for anonymous signing
    if(from_idn.public_key == NULL || to_idn.secret_key == NULL) return;

    encryptor_config_set_as_identity(app->encryptor_config, from_idn);
    encryptor_config_set_to_identity(app->encryptor_config, to_idn);

    uint8_t* decryption = encryptor_config_decrypt(app->encryptor_config);
    // set byte buffer to this pointer and also test if encryption is correct and decryptable

    if(decryption == NULL) return;
    app->export_data = decryption;
    app->export_size = encryptor_config_get_size(app->encryptor_config);

    scene_manager_handle_custom_event(app->scene_manager, 0);
}

// variable_item_list select with <your identity> <their identity> ([export] -> universal export screen)
void keyhold_scene_on_enter_decryptionconfig(void* ctx) {
    App* app = ctx;
    variable_item_list_reset(app->view_variableitemlist);

    // make VariableItem of all identities,
    // TODO: only owned identites should show up in sign as /encrypt as

    size_t n_saves = saves_get_number_of_saves(app->saves);

    VariableItem* from_selection = variable_item_list_add(
        app->view_variableitemlist,
        "Encryptor key (f: anon)",
        n_saves,
        keyhold_callback_change_fromR,
        app); // app is probably not correct ctx unless VariableItem contains this in it
    VariableItem* to_selection = variable_item_list_add(
        app->view_variableitemlist,
        "Your key",
        n_saves,
        keyhold_callback_change_toR,
        app); // app is probably not correct ctx unless VariableItem contains this in it

    if(n_saves > 0) {
        const char* first_save = saves_get_save_at(app->saves, 0);
        variable_item_set_current_value_index(from_selection, 0);
        variable_item_set_current_value_index(to_selection, 0);
        variable_item_set_current_value_text(from_selection, first_save);
        variable_item_set_current_value_text(to_selection, first_save);
    }

    variable_item_list_add(
        app->view_variableitemlist,
        "Decrypt & Export",
        0,
        keyhold_callback_change_fromR,
        NULL); // Button, change callback should not be run
    variable_item_list_set_enter_callback(
        app->view_variableitemlist, keyhold_callback_dencrypt, app);

    view_dispatcher_switch_to_view(app->vp, KeyholdViewVariableItemList);
}
bool keyhold_scene_on_event_decryptionconfig(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;

    bool consumed = false;

    if(evt.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, KeyholdSceneExportScreen);
        consumed = true;
    }

    return consumed;
}
void keyhold_scene_on_exit_decryptionconfig(void* ctx) {
    App* app = ctx;
    app->selector_names.name1 = 0;
    app->selector_names.name2 = 0;
    variable_item_list_reset(app->view_variableitemlist);
}
