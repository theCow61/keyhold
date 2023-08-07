#include <furi.h>
#include "encryptionscreen_scene.h"
#include "../keyhold.h"

void keyhold_callback_buttons(GuiButtonType result, InputType type, void* ctx) {
    UNUSED(type);
    App* app = ctx;
    switch(result) {
    case GuiButtonTypeLeft:
        scene_manager_handle_custom_event(app->scene_manager, GuiButtonTypeLeft);
        break;
    case GuiButtonTypeCenter:
    case GuiButtonTypeRight:
        scene_manager_handle_custom_event(app->scene_manager, GuiButtonTypeRight);
    default:
        break;
    }
}

void keyhold_scene_on_enter_encryptionscreen(void* ctx) {
    App* app = ctx;
    widget_reset(app->view_widget);

    widget_add_button_element(
        app->view_widget, GuiButtonTypeLeft, "Config", keyhold_callback_buttons, app);

    widget_add_button_element(
        app->view_widget, GuiButtonTypeRight, "Encrypt and Export", keyhold_callback_buttons, app);

    // config - encrypt - export

    view_dispatcher_switch_to_view(app->vp, KeyholdViewWidget);
}
bool keyhold_scene_on_event_encryptionscreen(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;
    bool consumed = false;

    switch(evt.type) {
    case SceneManagerEventTypeCustom:
        switch(evt.event) {
        case GuiButtonTypeLeft:
            scene_manager_next_scene(app->scene_manager, KeyholdSceneConfig);
            break;
        case GuiButtonTypeCenter:
            break;
        case GuiButtonTypeRight:
            scene_manager_next_scene(app->scene_manager, KeyholdSceneExportScreen);
        default:
            break;
        }
    default:
        break;
    }

    return consumed;
}
void keyhold_scene_on_exit_encryptionscreen(void* ctx) {
    App* app = ctx;
    widget_reset(app->view_widget);
}