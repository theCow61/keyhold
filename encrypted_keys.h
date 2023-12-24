#pragma once

#include "saves.h"
#include <storage/storage.h>

void encrypted_keys_encrypt_store_recordize(Saves* saves, Storage* storage, uint8_t* encryptor_key);
void encrypted_keys_decrypt_recordize(Saves* saves, Storage* storage, uint8_t* encryptor_key);
