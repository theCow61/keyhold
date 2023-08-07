#include "config_scene.h"
#include "../keyhold.h"

void keyhold_scene_on_enter_config(void* ctx) {
    App* app = ctx;
    variable_item_list_reset(app->view_variableitemlist);

    view_dispatcher_switch_to_view(app->vp, KeyholdViewVariableItemList);
}
bool keyhold_scene_on_event_config(void* ctx, SceneManagerEvent evt) {
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}
void keyhold_scene_on_exit_config(void* ctx) {
    App* app = ctx;
    variable_item_list_reset(app->view_variableitemlist);
}