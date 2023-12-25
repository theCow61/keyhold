#include "../keyhold.h"
#include "bluetooth_authentication_scene.h"
#include "encrypted_keys.h"
#include "gui/canvas.h"
#include "gui/modules/widget.h"
#include "gui/scene_manager.h"
#include "gui/view_dispatcher.h"
#include "services/serial_service.h"
#include <bt/bt_service/bt.h>
#include <furi_hal_bt.h>
#include <furi_hal_bt_serial.h>


uint16_t bt_serial_event_callback(SerialServiceEvent evt, void* ctx) {
    App* app = ctx;
    
    if (evt.event == SerialServiceEventTypeDataReceived && evt.data.size == 32) {
        furi_hal_bt_serial_set_event_callback(0, NULL, NULL); // Don't want evt.data.buffer to change after this until atleast after authentication
        // encrypted_keys_encrypt_store_recordize(app, evt.data.buffer);
        encrypted_keys_decrypt_recordize(app, evt.data.buffer);
        scene_manager_handle_custom_event(app->scene_manager, 0);
    }

    // FURI_LOG_RAW_D("\r\n");
    // for(size_t i = 0; i < evt.data.size; i++) {
    //     FURI_LOG_RAW_D("%02x", evt.data.buffer[i]);
    // }
    // FURI_LOG_RAW_D("\r\n");

    return 0;
}

void keyhold_scene_on_enter_bluetooth_authentication(void* ctx) {
    
    App* app = ctx;

    furi_record_open(RECORD_BT);
    furi_hal_bt_serial_start();

    widget_reset(app->view_widget);
    view_dispatcher_switch_to_view(app->vp, KeyholdViewWidget);
    widget_add_text_box_element(app->view_widget, 0, 0, 128, 50, AlignLeft, AlignTop, "Send auth key on BT serial characterstic. BT connection needs to be active pre entering this screen", false);

    // BT needs to be established before we enter the scene?
    furi_hal_bt_serial_set_event_callback(32, bt_serial_event_callback, app);

}
bool keyhold_scene_on_event_bluetooth_authentication(void* ctx, SceneManagerEvent evt) {
    App* app = ctx;

    bool consumed = false;

    if (evt.event == SceneManagerEventTypeCustom) {
        scene_manager_previous_scene(app->scene_manager); // Should be the main menu scene
        consumed = true;
    }

    return consumed;
}
void keyhold_scene_on_exit_bluetooth_authentication(void* ctx) {
    // furi_hal_bt_serial_stop(); // null pointer deref
    furi_hal_bt_serial_set_event_callback(0, NULL, NULL);
    furi_record_close(RECORD_BT);
    App* app = ctx;
    widget_reset(app->view_widget);
}
