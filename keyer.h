#pragma once

#include <storage/storage.h>

typedef struct Identity {
    uint8_t* secret_key;
    uint8_t* public_key;
} Identity;

/**
 * Pass NULL for any absent keys.
*/
Identity keyer_identity_init(uint8_t* sk, uint8_t* pk);
void keyer_identity_clear(Identity* idn);

Identity keyer_generate_keypair();

bool keyer_save_identity(Identity* idn, Storage* storage, char* name);

bool keyer_generate_key_pair_and_save(char* name, Storage* storage);
