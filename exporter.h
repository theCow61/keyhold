#pragma once

#include <storage/storage.h>

typedef enum {
    ExporterMessageTypeNull,
    ExporterMessageTypeEncryptedMessage,
    ExporterMessageTypeAnonymousEncryptedMessage,
    ExporterMessageTypeIdentity,
    ExporterMessageTypeContinuation = 9
} ExporterMessageType;

FuriString* exporter_save_buffer_as_file(uint8_t* buf, size_t size, Storage* storage);

void exporter_broadcast_buffer(
    SubGhzTxRxWorker* worker,
    uint8_t* buf,
    size_t size,
    ExporterMessageType msg_type);