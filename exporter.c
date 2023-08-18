#include <storage/storage.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include "exporter.h"
#include <lib/subghz/devices/registry.h>
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

#define RF_MESSAGE_MAX_LEN 64
#define RF_TAG "KH00:"
#define RF_AVAILABLE_SIZE RF_MESSAGE_MAX_LEN - sizeof(RF_TAG)

#define PATH                         \
    STORAGE_APP_DATA_PATH_PREFIX "/" \
                                 "saved_files"

FuriString* exporter_save_buffer_as_file(uint8_t* buf, size_t size, Storage* storage) {
    storage_common_mkdir(storage, PATH);
    FuriString* file_path = furi_string_alloc_printf("%s/saved_file0", PATH);
    int i = 1;
    while(storage_file_exists(storage, furi_string_get_cstr(file_path))) {
        furi_string_reset(file_path);
        furi_string_printf(file_path, "%s/saved_file%d", PATH, i++);
    }

    const char* const saved_path = furi_string_get_cstr(file_path);
    FURI_LOG_I("keyhold", "%s", saved_path);
    FURI_LOG_I("keyhold", "%s", saved_path);

    File* export_save_file = storage_file_alloc(storage);
    if(!storage_file_open(export_save_file, saved_path, FSAM_WRITE, FSOM_CREATE_NEW)) {
        storage_file_free(export_save_file);
        furi_string_reset(file_path);
        furi_string_set_str(file_path, "failed");
        return file_path;
    }

    if(!storage_file_write(export_save_file, buf, size)) {
        storage_file_close(export_save_file);
        storage_file_free(export_save_file);
        furi_string_reset(file_path);
        furi_string_set_str(file_path, "failed");
        return file_path;
    }
    storage_file_close(export_save_file);
    storage_file_free(export_save_file);
    return file_path;
}

void exporter_broadcast_buffer(
    SubGhzTxRxWorker* worker,
    uint8_t* buf,
    size_t size,
    ExporterMessageType msg_type) {
    const SubGhzDevice* dev = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    // keep in mind _ext

    if(!subghz_tx_rx_worker_start(worker, dev, 433920000)) {
        return;
    }

    furi_hal_light_blink_start(LightBlue, 155, 500, 0);

    size_t size_left = size;
    size_t message_fill_amount;
    ExporterMessageType type = msg_type;

    while(size_left > 0) {
        uint8_t part_buf[M_MIN(size_left + 5, RF_MESSAGE_MAX_LEN)];
        part_buf[0] = 'K';
        part_buf[1] = 'Y';
        part_buf[2] = '0';
        part_buf[3] = type;
        type = ExporterMessageTypeContinuation;
        part_buf[4] = size_left / RF_AVAILABLE_SIZE + 1;

        message_fill_amount = M_MIN(size_left, RF_AVAILABLE_SIZE);
        memcpy(part_buf + 5, buf, message_fill_amount);
        buf += message_fill_amount;
        size_left -= message_fill_amount;
        FURI_LOG_D("keyhold", "%.*s\nam: %d", message_fill_amount, part_buf, message_fill_amount);
        while(!subghz_tx_rx_worker_write(worker, part_buf, message_fill_amount + 5)) {
            furi_delay_ms(20);
        }
    }
    subghz_tx_rx_worker_stop(worker);
    furi_hal_light_blink_stop();
}
