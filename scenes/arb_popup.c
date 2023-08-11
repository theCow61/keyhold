#include "arb_popup.h"
#include "../keyhold.h"

void keyhold_scene_on_enter_arbpopup(void* ctx) {
    App* app = ctx;
    popup_reset(app->view_popup);
    popup_set_text(
        app->view_popup,
        furi_string_get_cstr(app->popup_text),
        182 / 2,
        64 / 2,
        AlignCenter,
        AlignCenter);
    view_dispatcher_switch_to_view(app->vp, KeyholdViewPopup);
}
bool keyhold_scene_on_event_arbpopup(void* ctx, SceneManagerEvent evt) {
    UNUSED(evt);
    App* app = ctx;
    scene_manager_previous_scene(app->scene_manager);
    return true;
}
void keyhold_scene_on_exit_arbpopup(void* ctx) {
    App* app = ctx;
    popup_reset(app->view_popup);
}