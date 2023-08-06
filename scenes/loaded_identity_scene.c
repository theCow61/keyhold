#include "loaded_identity_scene.h"

#include "../keyhold.h"

void keyhold_callback_loadedidentity_save(GuiButtonType result, InputType type, void* ctx) {
    UNUSED(result);
    UNUSED(type);

    App* app = ctx;
    // app->return_scene_buffer = KeyholdSceneLoadedIdentity;

    scene_manager_handle_custom_event(
        app->scene_manager, GuiButtonTypeRight); // clicked right button save
}

void keyhold_scene_on_enter_loadedidentity(void* ctx) {
    App* app = ctx;
    if(app->loaded_identity.secret_key == NULL) { // non-owned.
    }

    widget_reset(app->view_widget);

    widget_add_button_element(
        app->view_widget, GuiButtonTypeRight, "Save", keyhold_callback_loadedidentity_save, app);

    // export - show key, nfc key, save as file, etc

    // named identities - later

    view_dispatcher_switch_to_view(app->vp, KeyholdViewWidget);
}

bool keyhold_scene_on_event_loadedidentity(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;
    bool consumed = false;

    switch(evt.type) {
    case SceneManagerEventTypeCustom:
        switch(evt.event) {
        case GuiButtonTypeRight:
            scene_manager_next_scene(app->scene_manager, KeyholdSceneNamer);
            consumed = true;
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

void keyhold_scene_on_exit_loadedidentity(void* ctx) {
    App* app = ctx;
    widget_reset(app->view_widget);
}