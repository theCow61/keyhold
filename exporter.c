#include <storage/storage.h>
#include "exporter.h"

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