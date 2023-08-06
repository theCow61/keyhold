#pragma once

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
#include "scenes/main_menu_scene.h"
#include "scenes/loaded_identity_scene.h"
#include "scenes/namer_scene.h"
#include "keyer.h"

typedef enum {
    KeyholdSceneMainMenu,
    KeyholdSceneNamer,
    KeyholdSceneLoadedIdentity,
    KeyholdSceneCount,
} KeyholdScene;

typedef enum {
    KeyholdViewSubmenu,
    KeyholdViewFileBrowser,
    KeyholdViewTextInput,
    KeyholdViewPopup,
    KeyholdViewWidget
} KeyholdView;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* vp;
    Submenu* view_submenu;
    FileBrowser* view_filebrowser;
    TextInput* view_textinput;
    Popup* view_popup;
    Widget* view_widget;
    Storage* storage;
    char all_purpose_string[30];
    // KeyholdScene return_scene_buffer;
    Identity loaded_identity;
} App;
