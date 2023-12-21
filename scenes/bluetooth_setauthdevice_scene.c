#include "bluetooth_setauthdevice_scene.h"
#include "../keyhold.h"
#include "core/log.h"
#include "furi_hal_random.h"
#include "gui/modules/widget.h"
#include "gui/modules/widget_elements/widget_element.h"
#include "gui/scene_manager.h"
#include <furi_hal_bt.h>
#include <furi_hal_bt_serial.h>
#include <furi_hal_random.h>
#include <bt/bt_service/bt.h>
#include "lib/monocypher/monocypher.h"
#include "../keyer.h"
#include "../saves.h"

void proceed_onclicked(GuiButtonType result, InputType type, void* context) {
    UNUSED(type);
    App* app = context;
    switch (result) {
        case GuiButtonTypeRight:
        // Bluetooth
        scene_manager_handle_custom_event(app->scene_manager, 0);
        break;
        case GuiButtonTypeLeft:
        // NFC
        scene_manager_handle_custom_event(app->scene_manager, 1);
        default:
            break;
    }
}


uint8_t* key;
void keyhold_scene_on_enter_bluetooth_setauthdevice(void* ctx) {
    App* app = ctx;

    widget_reset(app->view_widget);

    view_dispatcher_switch_to_view(app->vp, KeyholdViewWidget);
    

    Bt* bt = furi_record_open(RECORD_BT);
    UNUSED(bt);
    furi_hal_bt_serial_start();

    uint8_t auth_key[32];
    key = auth_key;
    furi_hal_random_fill_buf(auth_key, 32);
    for (int i = 0; i < 32; i++) {
        FURI_LOG_RAW_D("%02X ", auth_key[i]);
    }
    FURI_LOG_D("keyhold", "\r\n");


    widget_add_button_element(app->view_widget, GuiButtonTypeRight, "Proceed?", proceed_onclicked, app);
    widget_add_button_element(app->view_widget, GuiButtonTypeLeft, "NFC", proceed_onclicked, app);
    widget_add_text_box_element(app->view_widget, 0, 0, 128, 50, AlignLeft, AlignTop, "Auth key on BT serial characterstic. Proceeding will encrypt private keys; have auth key saved before. Or switch auth method to NFC.", false);


    // encrypt all saves
    // make sure bluetooth only should work if passcode pairing has been done or do passcode pairing
    // Add NFC authenticator; store key on NFC.


    bool r = furi_hal_bt_serial_tx(auth_key, 32); // may need to use bt_keys. recall /int/.bt.keys
    FURI_LOG_D("keyhold", "R: %d", r);

}

bool keyhold_scene_on_event_bluetooth_setauthdevice(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;
    UNUSED(app);
    bool consumed = false;

    switch (evt.type) {
        case SceneManagerEventTypeCustom:
            switch (evt.event) {
                case 0:
                // Bluetooth
                    // scene_manager_next_scene
                    consumed = true;
                    // debug log reports from tag "BtSerialSvc" that "Failed updating TX charactersitic: 12", but it still works. If causes issues in future use buffer (uint8_t [32]) {0} as that doesn't seem to produce this error.
                    uint8_t emptier[5] = { 4, 8, 0x23, 0x22, 0 }; // values after flipper app loads, so it may be concluded that the flipper's last bluetooth interaction was with flipper app and not our application.
                    furi_hal_bt_serial_tx(emptier, 5);
                    crypto_wipe(key, 32);
                    
                    ram_store_and_encrypt(app);
                    break;
                case 1:
                // NFC
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
void keyhold_scene_on_exit_bluetooth_setauthdevice(void* ctx) {
    App* app = ctx;
    furi_record_close(RECORD_BT);
    widget_reset(app->view_widget);
}
