#pragma once

#include <storage/storage.h>

#define SAVED_FILE STORAGE_APP_DATA_PATH_PREFIX "/.saved"

typedef struct Saves Saves;

bool saves_add(Saves* saves, const char* save);
Saves* saves_alloc();
void saves_free(Saves* saves);
Saves* saves_alloc_load(Storage* storage);
const char* saves_get_save_at(Saves* saves, size_t idx);
size_t saves_get_number_of_saves(Saves* saves);

