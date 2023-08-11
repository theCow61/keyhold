#include "export_screen.h"
#include "../keyhold.h"
#include "../exporter.h"

void keyhold_callback_exportscreen(void* ctx, uint32_t idx) {
    App* app = ctx;

    switch(idx) {
    case 0:
        app->popup_text =
            exporter_save_buffer_as_file(app->export_data, app->export_size, app->storage);
        scene_manager_handle_custom_event(app->scene_manager, 0);
        break;
    default:
        break;
    }
}

void keyhold_scene_on_enter_exportscreen(void* ctx) {
    App* app = ctx;
    submenu_reset(app->view_submenu);
    submenu_set_header(app->view_submenu, "Select Export Option");
    submenu_add_item(app->view_submenu, "Save as File", 0, keyhold_callback_exportscreen, app);
    submenu_add_item(app->view_submenu, "RF Broadcast", 1, keyhold_callback_exportscreen, app);

    view_dispatcher_switch_to_view(app->vp, KeyholdViewSubmenu);
}
bool keyhold_scene_on_event_exportscreen(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;

    bool consumed = false;

    switch(evt.type) {
    case SceneManagerEventTypeCustom:
        switch(evt.event) {
        case 0: // save as file
            scene_manager_next_scene(app->scene_manager, KeyholdSceneArbPopup);
            // pop up "Saved at: "
            consumed = true;
            break;
        default:
            break;
        }
        break;
    case SceneManagerEventTypeBack:
        scene_manager_search_and_switch_to_another_scene(app->scene_manager, KeyholdSceneMainMenu);
        consumed = true;
        break;
    default:
        break;
    }

    return consumed;
}
void keyhold_scene_on_exit_exportscreen(void* ctx) {
    App* app = ctx;
    submenu_reset(app->view_submenu);
}