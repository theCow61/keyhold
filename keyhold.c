#include <furi.h>
#include <gui/gui.h>
#include <storage/storage.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/popup.h>
#include "keyer.h"
#include "keyhold.h"

// enums once here

// app struct was once here

/*
Start Main Menu
*/

/*
End Main Menu
*/

/*
Start Generate Key Pair
*/

/*
End Generate Key Pair
*/

/*
Start OK Scene
*/

// void keyhold_callback_ok(void* ctx) {
//     App* app = ctx;
//     scene_manager_handle_custom_event(app->scene_manager, 0);
// }

// void keyhold_scene_on_enter_ok(void* ctx) {
//     App* app = ctx;
//     popup_reset(app->view_popup);
//     popup_set_header(app->view_popup, "Ok", 40, 40, AlignCenter, AlignCenter);
//     popup_set_context(app->view_popup, app);
//     popup_set_callback(app->view_popup, keyhold_callback_ok);
//     view_dispatcher_switch_to_view(app->vp, KeyholdViewPopup);
// }

// bool keyhold_scene_on_event_ok(void* ctx, SceneManagerEvent evt) {
//     UNUSED(evt);
//     App* app = ctx;

//     scene_manager_next_scene(app->scene_manager, KeyholdSceneMainMenu);

//     return true;
// }

// void keyhold_scene_on_exit_ok(void* ctx) {
//     App* app = ctx;
//     popup_reset(app->view_popup);
// }
/*
End OK Scene
*/

/*
Start Error Scene
*/

// void keyhold_callback_error(void* ctx) {
//     App* app = ctx;
//     scene_manager_handle_custom_event(app->scene_manager, 0);
// }

// void keyhold_scene_on_enter_error(void* ctx) {
//     App* app = ctx;
//     popup_reset(app->view_popup);
//     popup_set_header(app->view_popup, "Error", 40, 40, AlignCenter, AlignCenter);
//     popup_set_context(app->view_popup, app);
//     popup_set_callback(app->view_popup, keyhold_callback_error);
//     view_dispatcher_switch_to_view(app->vp, KeyholdViewPopup);
// }

// bool keyhold_scene_on_event_error(void* ctx, SceneManagerEvent evt) {
//     UNUSED(evt);
//     App* app = ctx;

//     scene_manager_next_scene(app->scene_manager, KeyholdSceneMainMenu);

//     return true;
// }

// void keyhold_scene_on_exit_error(void* ctx) {
//     App* app = ctx;
//     popup_reset(app->view_popup);
// }

/*
End Error Scene
*/

// handlers here

void (*const keyhold_scene_on_enter_handlers[])(void*) = {
    keyhold_scene_on_enter_mainmenu,
    keyhold_scene_on_enter_namer,
    keyhold_scene_on_enter_loadedidentity,
    keyhold_scene_on_enter_exportscreen,
    keyhold_scene_on_enter_config,
    keyhold_scene_on_enter_encryptionmessage,
    keyhold_scene_on_enter_encryptionconfig,
    keyhold_scene_on_enter_decryptionfile,
    keyhold_scene_on_enter_filesaver,
    keyhold_scene_on_enter_arbpopup,
    keyhold_scene_on_enter_decryptionconfig,
};

bool (*const keyhold_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    keyhold_scene_on_event_mainmenu,
    keyhold_scene_on_event_namer,
    keyhold_scene_on_event_loadedidentity,
    keyhold_scene_on_event_exportscreen,
    keyhold_scene_on_event_config,
    keyhold_scene_on_event_encryptionmessage,
    keyhold_scene_on_event_encryptionconfig,
    keyhold_scene_on_event_decryptionfile,
    keyhold_scene_on_event_filesaver,
    keyhold_scene_on_event_arbpopup,
    keyhold_scene_on_event_decryptionconfig,
};

void (*const keyhold_scene_on_exit_handlers[])(void*) = {
    keyhold_scene_on_exit_mainmenu,
    keyhold_scene_on_exit_namer,
    keyhold_scene_on_exit_loadedidentity,
    keyhold_scene_on_exit_exportscreen,
    keyhold_scene_on_exit_config,
    keyhold_scene_on_exit_encryptionmessage,
    keyhold_scene_on_exit_encryptionconfig,
    keyhold_scene_on_exit_decryptionfile,
    keyhold_scene_on_exit_filesaver,
    keyhold_scene_on_exit_arbpopup,
    keyhold_scene_on_exit_decryptionconfig,
};

static const SceneManagerHandlers keyhold_scene_manager_handlers = {
    .on_enter_handlers = keyhold_scene_on_enter_handlers,
    .on_event_handlers = keyhold_scene_on_event_handlers,
    .on_exit_handlers = keyhold_scene_on_exit_handlers,
    .scene_num = KeyholdSceneCount,
};

static bool keyhold_custom_callback(void* ctx, uint32_t custom_event) {
    furi_assert(ctx);
    App* app = ctx;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool keyhold_back_event_callback(void* ctx) {
    furi_assert(ctx);
    App* app = ctx;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->storage = furi_record_open(RECORD_STORAGE);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->saves = saves_alloc_load(app->storage);

    app->scene_manager = scene_manager_alloc(&keyhold_scene_manager_handlers, app);
    app->vp = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->vp);
    view_dispatcher_set_event_callback_context(app->vp, app);
    view_dispatcher_set_custom_event_callback(app->vp, keyhold_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->vp, keyhold_back_event_callback);
    app->view_submenu = submenu_alloc();
    view_dispatcher_add_view(app->vp, KeyholdViewSubmenu, submenu_get_view(app->view_submenu));
    app->view_filebrowser = file_browser_alloc(furi_string_alloc_printf("/"));
    view_dispatcher_add_view(
        app->vp, KeyholdViewFileBrowser, file_browser_get_view(app->view_filebrowser));

    app->view_textinput = text_input_alloc();
    view_dispatcher_add_view(
        app->vp, KeyholdViewTextInput, text_input_get_view(app->view_textinput));

    app->view_popup = popup_alloc();
    view_dispatcher_add_view(app->vp, KeyholdViewPopup, popup_get_view(app->view_popup));

    app->view_widget = widget_alloc();
    view_dispatcher_add_view(app->vp, KeyholdViewWidget, widget_get_view(app->view_widget));

    app->view_variableitemlist = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->vp,
        KeyholdViewVariableItemList,
        variable_item_list_get_view(app->view_variableitemlist));

    app->loaded_identity = keyer_identity_init(NULL, NULL);
    app->encryptor_config = encryptor_config_alloc();
    app->selector_names = (SelectorNames){0, 0};
    app->popup_text = NULL;
    app->file_browser_path = furi_string_alloc();
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    keyer_identity_clear(&app->loaded_identity);
    encryptor_config_free(app->encryptor_config);
    view_dispatcher_remove_view(app->vp, KeyholdViewSubmenu);
    view_dispatcher_remove_view(app->vp, KeyholdViewFileBrowser);
    view_dispatcher_remove_view(app->vp, KeyholdViewTextInput);
    view_dispatcher_remove_view(app->vp, KeyholdViewPopup);
    view_dispatcher_remove_view(app->vp, KeyholdViewWidget);
    view_dispatcher_remove_view(app->vp, KeyholdViewVariableItemList);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->vp);
    submenu_free(app->view_submenu);
    file_browser_free(app->view_filebrowser);
    text_input_free(app->view_textinput);
    popup_free(app->view_popup);
    widget_free(app->view_widget);
    variable_item_list_free(app->view_variableitemlist);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    // saves_free(app->saves);
    furi_string_free(app->file_browser_path);
    free(app);
}

uint8_t keyhold_app(void* p) {
    UNUSED(p);

    App* app = app_alloc();

    // keyhold_load_save(app->storage);

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->vp, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, KeyholdSceneMainMenu);
    view_dispatcher_run(app->vp);

    // keyhold_unload_save();
    app_free(app);
    return 0;
}
