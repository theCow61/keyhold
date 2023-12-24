#include "namer_scene.h"
#include "../keyhold.h"

void keyhold_callback_savename(void* ctx) {
    App* app = ctx;
    bool result = keyer_save_identity(
        &app->loaded_identity, app->saves, app->storage, app->all_purpose_string);
    scene_manager_handle_custom_event(app->scene_manager, result);
}

void keyhold_scene_on_enter_namer(void* ctx) {
    App* app = ctx;
    text_input_reset(app->view_textinput);
    text_input_set_header_text(app->view_textinput, "Name Identity");

    text_input_set_result_callback(
        app->view_textinput, keyhold_callback_savename, app, app->all_purpose_string, 11, true

    );
    view_dispatcher_switch_to_view(app->vp, KeyholdViewTextInput);
}
bool keyhold_scene_on_event_namer(void* ctx, SceneManagerEvent evt) {
    // UNUSED(ctx);
    App* app = ctx;
    bool consumed = false;

    switch(evt.type) {
    case SceneManagerEventTypeCustom:
        switch(evt.event) {
        case true:
            // scene_manager_next_scene(app->scene_manager, KeyholdSceneOk);
            scene_manager_next_scene(app->scene_manager, KeyholdSceneLoadedIdentity); //original
            consumed = true;
            break;
        case false:
            scene_manager_next_scene(app->scene_manager, KeyholdSceneMainMenu);
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
void keyhold_scene_on_exit_namer(void* ctx) {
    App* app = ctx;
    text_input_reset(app->view_textinput);
    // deal with text_buffer
}
