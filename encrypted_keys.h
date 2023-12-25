#pragma once

#include "keyhold.h"
#include <storage/storage.h>

void encrypted_keys_encrypt_store_recordize(App* app, uint8_t* encryptor_key);
void encrypted_keys_decrypt_recordize(App* app, uint8_t* encryptor_key);
