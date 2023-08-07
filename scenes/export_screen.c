#include "export_screen.h"
#include "../keyhold.h"

void keyhold_callback_exportscreen(void* ctx, uint32_t idx) {
    UNUSED(ctx);
    UNUSED(idx);
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
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}
void keyhold_scene_on_exit_exportscreen(void* ctx) {
    App* app = ctx;
    submenu_reset(app->view_submenu);
}