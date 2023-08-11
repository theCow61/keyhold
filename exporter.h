#pragma once

#include <storage/storage.h>

FuriString* exporter_save_buffer_as_file(uint8_t* buf, size_t size, Storage* storage);