#include "encrypted_keys.h"
#include "keyer.h"

void encrypted_keys_encrypt_store_recordize(Saves* saves, Storage* storage, uint8_t* encryptor_key) {
    
    size_t nSaves = saves_get_number_of_saves(saves);



    uint8_t** encrypted_keys = malloc(nSaves * sizeof(uint8_t*));


    for (size_t i = 0; i < nSaves; i++) {
        const char* save_at_i = saves_get_save_at(saves, i);
        Identity idn_at_i = keyer_get_identity(storage, save_at_i);

        // lets hope this person doesn't have too many friends. An array of a bunch of null pointers floating in flipper at all time is not ideal. Option could be to make hashmap.
        encrypted_keys[i] = idn_at_i.secret_key;
        if (idn_at_i.secret_key == NULL) { // this save is not an owned identity then
            continue; // we don't want to encrypt it
        }

        // keyer_encrypt_sk_file

        keyer_encrypt_x25509_key_file_chacha20(storage, encryptor_key, save_at_i, idn_at_i.secret_key);
    }

    // the same saves can be used 
    // REMEMBER: Deleting a save must also delete its record here to keep its indexing lined up. Try to find better way of doing all of this, like a hash map of owned identities.

    furi_record_create("RECORDS_KEYHOLD", encrypted_keys);

}
