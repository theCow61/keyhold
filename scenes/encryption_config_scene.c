#include "encryption_config_scene.h"

#include "../keyhold.h"

// variable_item_list select with <your identity> <their identity> ([export] -> universal export screen)
void keyhold_scene_on_enter_encryptionconfig(void* ctx) {
    UNUSED(ctx);
}
bool keyhold_scene_on_event_encryptionconfig(void* ctx, SceneManagerEvent evt) {
    UNUSED(ctx);
    UNUSED(evt);
    return false;
}
void keyhold_scene_on_exit_encryptionconfig(void* ctx) {
    UNUSED(ctx);
}