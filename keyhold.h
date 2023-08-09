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
#include <gui/modules/variable_item_list.h>
#include "scenes/main_menu_scene.h"
#include "scenes/loaded_identity_scene.h"
#include "scenes/namer_scene.h"
#include "scenes/encryptionscreen_scene.h"
#include "scenes/export_screen.h"
#include "scenes/config_scene.h"
#include "scenes/encryption_message.h"
#include "scenes/encryption_config_scene.h"
#include "keyer.h"
#include "saves.h"

typedef enum {
    KeyholdSceneMainMenu,
    KeyholdSceneNamer,
    KeyholdSceneLoadedIdentity,
    KeyholdSceneEncryptionScreen,
    KeyholdSceneExportScreen,
    KeyholdSceneConfig,
    KeyholdSceneEncryptionMessage,
    KeyholdSceneEncryptionConfig,
    KeyholdSceneCount,
} KeyholdScene;

typedef enum {
    KeyholdViewSubmenu,
    KeyholdViewFileBrowser,
    KeyholdViewTextInput,
    KeyholdViewPopup,
    KeyholdViewWidget,
    KeyholdViewVariableItemList,
} KeyholdView;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* vp;
    Submenu* view_submenu;
    FileBrowser* view_filebrowser;
    TextInput* view_textinput;
    Popup* view_popup;
    Widget* view_widget;
    VariableItemList* view_variableitemlist;
    Storage* storage;
    char all_purpose_string[255];
    // KeyholdScene return_scene_buffer;
    Identity loaded_identity;
    uint8_t* export_data;
    Saves* saves;
    EncryptorConfig* encryptor_config;
    // Export export used to represent any buffer that needs to be exported as file or nfc or watever
} App;
