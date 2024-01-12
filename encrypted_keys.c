#include "encrypted_keys.h"
#include "keyer.h"
#include "keyhold.h"
#include "storage/filesystem_api_defines.h"
#include <storage/storage.h>
#include "lib/monocypher/monocypher.h"

void hex_printer(uint8_t* byte_buf, size_t size) {
        FURI_LOG_RAW_D("\r\n");
        for (size_t i = 0; i < size; i++) {
            FURI_LOG_RAW_D("%02x ", byte_buf[i]);
        }
        FURI_LOG_RAW_D("\r\n");
}

void encrypted_keys_encrypt_store_recordize(App* app, uint8_t* encryptor_key) {

    Saves* saves = app->saves;
    Storage* storage = app->storage;
    
    size_t nSaves = saves_get_number_of_saves(saves);



    uint8_t** encrypted_keys = malloc(nSaves * sizeof(uint8_t*));


    for (size_t i = 0; i < nSaves; i++) {
        const char* save_at_i = saves_get_save_at(saves, i);
        Identity idn_at_i = keyer_get_identity(storage, save_at_i);

        // lets hope this person doesn't have too many friends. An array of a bunch of null pointers floating in flipper at all time is not ideal. Option could be to make hashmap.
        encrypted_keys[i] = idn_at_i.secret_key;
        if (idn_at_i.secret_key == NULL) { // this save is not an owned identity then
            encrypted_keys[i] = NULL;
            continue; // we don't want to encrypt it
        }

        // keyer_encrypt_sk_file

        keyer_encrypt_x25509_key_file_chacha20(storage, encryptor_key, save_at_i, idn_at_i.secret_key);

    }

    // the same saves can be used 
    // REMEMBER: Deleting a save must also delete its record here to keep its indexing lined up. Try to find better way of doing all of this, like a hash map of owned identities.

    furi_record_create("RECORDS_KEYHOLD", encrypted_keys);
    app->encrypted_keys = furi_record_open("RECORDS_KEYHOLD");

}

// repeating almost basically same code to save on an uneeded branc
void encrypted_keys_decrypt_recordize(App* app, uint8_t* encryptor_key) {
    Saves* saves = app->saves;
    Storage* storage = app->storage;

    size_t nSaves = saves_get_number_of_saves(saves);

    uint8_t** encrypted_keys = malloc(nSaves * sizeof(uint8_t*));


    for (size_t i = 0; i < nSaves; i++) {
        const char* save_at_i = saves_get_save_at(saves, i);
        FURI_LOG_D("keyhold", "%s", save_at_i);
        Identity idn_at_i = keyer_get_identity(storage, save_at_i);
        free(idn_at_i.public_key); // make function that only pulls private key because we dont need public key here; wasting file read
        if (idn_at_i.secret_key == NULL) { // PROBLEM secret keys are null from files?
            encrypted_keys[i] = NULL;
            continue; // non owned identity or file read failed
        }

        encrypted_keys[i] = idn_at_i.secret_key;
        hex_printer(encrypted_keys[i], 32);


        // nonce shall overlap with key.
        // idn_at_i.secret_key is pulled from file that we are assuming has been ciphered
        // inplace encryption
        crypto_chacha20_x(encrypted_keys[i], encrypted_keys[i], 32, encryptor_key, encryptor_key, 0);


    }

    furi_record_create("RECORDS_KEYHOLD", encrypted_keys);
    app->encrypted_keys = furi_record_open("RECORDS_KEYHOLD");

}


