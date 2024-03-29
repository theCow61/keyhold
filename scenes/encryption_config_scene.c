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

void keyhold_callback_change_from(
    VariableItem* vi) { // we will render text as user filps through selection

    // Option 1
    App* app = variable_item_get_context(vi);

    uint8_t idx = variable_item_get_current_value_index(vi);
    variable_item_set_current_value_text(vi, saves_get_save_at(app->saves, idx));

    // This is needed as we can't retrieve our VariableItem in enter callback besides making a maybe memory leaking pointer holder
    app->selector_names.name1 = idx;
}

// We make 2 to avoid branching

void keyhold_callback_change_to(
    VariableItem* vi) { // we will render text as user filps through selection

    // Option 1
    App* app = variable_item_get_context(vi);

    uint8_t idx = variable_item_get_current_value_index(vi);
    variable_item_set_current_value_text(vi, saves_get_save_at(app->saves, idx));

    app->selector_names.name2 = idx;
}

// lol global variable who cares
int anon = 0;

void keyhold_callback_anon_switch(VariableItem* vi) {
  if (variable_item_get_current_value_index(vi) == 1) {
    variable_item_set_current_value_text(vi, "Y");
    anon = 1;
  } else {
    variable_item_set_current_value_text(vi, "N");
    anon = 0;
  }
}

void keyhold_callback_anon_encrypt(VariableItem* vi) {
    App* app = variable_item_get_context(vi);
    // false is 0 and true is 1 so this should line up
    app->epho_anon_mode = variable_item_get_current_value_index(vi);
}

void keyhold_callback_aencrypt(void* ctx, uint32_t idx) {
    if(idx != 3)
        return; // 0 is when clicked selector 1 is when clicked selector 2 is when clicked our button

    App* app = ctx;

    size_t n_saves = saves_get_number_of_saves(app->saves);
    if(n_saves == 0) return; // no saves

    const char* from_save = saves_get_save_at(app->saves, app->selector_names.name1);
    const char* to_save = saves_get_save_at(app->saves, app->selector_names.name2);

    Identity from_idn;
    if(!app->epho_anon_mode) {
        // from_idn = keyer_get_identity(app->storage, from_save);
        from_idn = keyer_get_correct_identity(app->storage, app->encrypted_keys, from_save, app->selector_names.name1);
    } else {
        from_idn = keyer_generate_keypair(); // generate random keypair
    }
    Identity to_idn = keyer_get_pub_identity(app->storage, to_save);

    // might have to modify for anonymous signing
    if(from_idn.secret_key == NULL || from_idn.public_key == NULL || to_idn.public_key == NULL ||
       encryptor_config_get_plain_buffer(app->encryptor_config) == NULL)
        return;

    encryptor_config_set_as_identity(app->encryptor_config, from_idn);
    encryptor_config_set_to_identity(app->encryptor_config, to_idn);

    uint8_t* encryption = encryptor_config_encrypt(app->encryptor_config);
    // set byte buffer to this pointer and also test if encryption is correct and decryptable

    app->export_data = encryption;
    app->export_size = encryptor_config_get_size(app->encryptor_config) + 24 + 16;

    scene_manager_handle_custom_event(app->scene_manager, 0);
}

// variable_item_list select with <your identity> <their identity> ([export] -> universal export screen)
void keyhold_scene_on_enter_encryptionconfig(void* ctx) {
    App* app = ctx;
    variable_item_list_reset(app->view_variableitemlist);

    // make VariableItem of all identities,
    // TODO: only owned identites should show up in sign as /encrypt as

    size_t n_saves = saves_get_number_of_saves(app->saves);

    VariableItem* from_selection = variable_item_list_add(
        app->view_variableitemlist,
        "Encrypt as",
        n_saves,
        keyhold_callback_change_from,
        app); // app is probably not correct ctx unless VariableItem contains this in it
    VariableItem* to_selection = variable_item_list_add(
        app->view_variableitemlist,
        "Encrypt for",
        n_saves,
        keyhold_callback_change_to,
        app); // app is probably not correct ctx unless VariableItem contains this in it

    VariableItem* anon_selector = variable_item_list_add(
        app->view_variableitemlist, "Anonymously?", 2, keyhold_callback_anon_switch, app);

    variable_item_set_current_value_index(anon_selector, 0);
    variable_item_set_current_value_text(anon_selector, "N");

    if(n_saves > 0) {
        const char* first_save = saves_get_save_at(app->saves, 0);
        variable_item_set_current_value_index(from_selection, 0);
        variable_item_set_current_value_index(to_selection, 0);
        variable_item_set_current_value_text(from_selection, first_save);
        variable_item_set_current_value_text(to_selection, first_save);
    }

    variable_item_list_add(
        app->view_variableitemlist,
        "Encrypt & Export",
        0,
        keyhold_callback_change_from,
        NULL); // Button, change callback should not be run
    variable_item_list_set_enter_callback(
        app->view_variableitemlist, keyhold_callback_aencrypt, app);

    app->epho_anon_mode = false;
    view_dispatcher_switch_to_view(app->vp, KeyholdViewVariableItemList);
}
bool keyhold_scene_on_event_encryptionconfig(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;

    bool consumed = false;

    if(evt.type == SceneManagerEventTypeCustom) {
        app->export_type = ExporterMessageTypeEncryptedMessage;
        scene_manager_next_scene(app->scene_manager, KeyholdSceneExportScreen);
        consumed = true;
    }

    return consumed;
}
void keyhold_scene_on_exit_encryptionconfig(void* ctx) {
    App* app = ctx;
    app->selector_names.name1 = 0;
    app->selector_names.name2 = 0;
    variable_item_list_reset(app->view_variableitemlist);
}
