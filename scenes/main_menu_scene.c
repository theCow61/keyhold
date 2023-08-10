#include "main_menu_scene.h"

#include "../keyhold.h"

typedef enum {
    KeyholdMainMenuEventGenerateIdentity,
    KeyholdMainMenuEventEncryptionMessage,
} KeyholdMainMenuEvent;

void keyhold_callback_menu(void* ctx, uint32_t idx) {
    App* app = ctx;
    switch(idx) {
    case 0:
        scene_manager_handle_custom_event(
            app->scene_manager, KeyholdMainMenuEventGenerateIdentity);
        break;
    case 1:
        scene_manager_handle_custom_event(
            app->scene_manager, KeyholdMainMenuEventEncryptionMessage);
        break;
    }
}

void keyhold_scene_on_enter_mainmenu(void* ctx) {
    App* app = ctx;
    keyer_identity_clear(&app->loaded_identity);
    encryptor_config_reset(app->encryptor_config);
    submenu_reset(app->view_submenu);
    submenu_set_header(app->view_submenu, "Key Hold");
    submenu_add_item(app->view_submenu, "Generate Key Pair", 0, keyhold_callback_menu, app);
    submenu_add_item(app->view_submenu, "Encrypt Message", 1, keyhold_callback_menu, app);
    submenu_add_item(app->view_submenu, "Encrypt File", 2, keyhold_callback_menu, app);
    view_dispatcher_switch_to_view(app->vp, KeyholdViewSubmenu);
}

bool keyhold_scene_on_event_mainmenu(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;

    bool consumed = false;

    switch(evt.type) {
    case SceneManagerEventTypeCustom:
        switch(evt.event) {
        case KeyholdMainMenuEventGenerateIdentity:
            scene_manager_next_scene(app->scene_manager, KeyholdSceneLoadedIdentity);
            // scene_manager_next_scene(app->scene_manager, KeyholdSceneGenerateKeyPair);
            consumed = true;
            break;
        case KeyholdMainMenuEventEncryptionMessage:
            consumed = true;
            scene_manager_next_scene(app->scene_manager, KeyholdSceneEncryptionMessage);
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void keyhold_scene_on_exit_mainmenu(void* ctx) {
    App* app = ctx;
    submenu_reset(app->view_submenu);
}
