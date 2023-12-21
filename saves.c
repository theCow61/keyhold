
#include "core/record.h"
#include <furi.h>
#include <storage/storage.h>
#include <stream/file_stream.h>
#include <stream/stream.h>
#include "saves.h"

#define SAVES_START_MAX 15
#define SAVES_MAX_UPDATE 6
#define SAVES_RECORD "Keyhold"

typedef struct Saves {
    const char** saved;
    size_t amount_of_saved;
    size_t max;
} Saves;

bool saves_add(Saves* saves, const char* save) {
    const char* new_str = strdup(save);

    if(saves->amount_of_saved == saves->max) { // resize
        saves->max += SAVES_MAX_UPDATE;
        const char** tmp;
        tmp = realloc(saves->saved, saves->max * sizeof(const char*));
        if(tmp == NULL) { //realloc failed
            free((char*)new_str);
            return false;
        }
        saves->saved = tmp;
    }

    saves->saved[saves->amount_of_saved++] = new_str;

    return true;
}

Saves* saves_alloc() {
    Saves* saves = malloc(sizeof(Saves));
    saves->saved = malloc(SAVES_START_MAX * sizeof(const char*));
    saves->amount_of_saved = 0;
    saves->max = SAVES_START_MAX;
    return saves;
}

void saves_free(Saves* saves) { // don't use this if SAVES_RECORD record exists our based on its implications
    FURI_LOG_D("keyhold", "am: %d", saves->amount_of_saved);
    for(size_t i = 0; i < saves->amount_of_saved; i++) {
        free((char*)saves->saved[i]);
    }
    free(saves->saved);
    free(saves);
}

Saves* saves_alloc_load(Storage* storage) {
    Saves* saves = saves_alloc();

    Stream* file_stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    if(file_stream_open(file_stream, SAVED_FILE, FSAM_READ, FSOM_OPEN_ALWAYS)) {
        while(stream_read_line(file_stream, line)) {
            furi_string_replace(line, "\r", "");
            furi_string_replace(line, "\n", "");
            const char* c_line = furi_string_get_cstr(line);
            if(!saves_add(saves, c_line)) {
                FURI_LOG_E("keyhold", "Failed to load %s\n", c_line);
            }
        }
    }
    furi_string_free(line);
    file_stream_close(file_stream);
    stream_free(file_stream);

    return saves;
}

const char* saves_get_save_at(Saves* saves, size_t idx) {
    return saves->saved[idx];
}

size_t saves_get_number_of_saves(Saves* saves) {
    return saves->amount_of_saved;
}

void saves_load_into_ram(Saves* saves) {
    if (furi_record_exists(SAVES_RECORD)) {
        return;
    }

    furi_record_create(SAVES_RECORD, saves);
    // when opening app, we need to check if this record exists, if it does, then add the saves data from record and put into saves data produced from files
}
