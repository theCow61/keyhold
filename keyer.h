#pragma once

#include <storage/storage.h>
#include "saves.h"

typedef struct Identity {
    uint8_t* secret_key;
    uint8_t* public_key;
} Identity;

typedef struct EncryptorConfig EncryptorConfig;

/**
 * Pass NULL for any absent keys.
*/
Identity keyer_identity_init(uint8_t* sk, uint8_t* pk);
void keyer_identity_clear(Identity* idn);

Identity keyer_generate_keypair();

bool keyer_save_identity(Identity* idn, Saves* saves, Storage* storage, char* name);

bool keyer_generate_key_pair_and_save(char* name, Storage* storage);

/**
 * grabs `Identity` from loaded saves at the given index.
 *
 * @param storage storage
 * @param index index in saves
 * @return corresponding `Identity`
 */
Identity keyer_get_identity(Storage* storage, const char* name);

/**
 * limits secret data being active in memory
 */
Identity keyer_get_pub_identity(Storage* storage, const char* name);

// void keyhold_load_save(Storage* storage);

// void keyhold_unload_save();

// bool saves_add(const char* save);

EncryptorConfig* encryptor_config_alloc();
void encryptor_config_reset(EncryptorConfig* ecf);
void encryptor_config_free(EncryptorConfig* ecf);

uint8_t* encryptor_config_get_plain_buffer(EncryptorConfig* ecf);
void encryptor_config_sync_pbuffer_str(EncryptorConfig* ecf, char* buf);

void encryptor_config_set_as_identity(EncryptorConfig* ecf, Identity idn);
void encryptor_config_set_to_identity(EncryptorConfig* ecf, Identity idn);

/**
 * config is mostly wiped post encrypt, you can only retrieve your encryption message and its size
 */
uint8_t* encryptor_config_encrypt(EncryptorConfig* ecf);

size_t encryptor_config_get_size(EncryptorConfig* ecf);

uint8_t* encryptor_config_encrypted_bytes_buf_init(EncryptorConfig* ecf, size_t size);

uint8_t* encryptor_config_decrypt(EncryptorConfig* ecf);

size_t str_len(char* str);


void keyer_encrypt_x25509_key_file_chacha20(Storage* storage, uint8_t* encryptor_key, const char* save_name, uint8_t* save_sk_key);
