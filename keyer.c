#include "keyer.h"
#include "lib/monocypher/monocypher.h"
#include <furi.h>
#include <furi_hal_random.h>
#include <storage/storage.h>

Identity keyer_identity_init(uint8_t* sk, uint8_t* pk) {
    return (Identity){.public_key = pk, .secret_key = sk};
}

void keyer_identity_clear(Identity* idn) {
    if(idn->secret_key != NULL) {
        crypto_wipe(idn->secret_key, 32);
    }
    if(idn->public_key != NULL) {
        crypto_wipe(idn->public_key, 32);
    }
    idn->secret_key = NULL;
    idn->public_key = NULL;
}

bool keyer_generate_key_pair_and_save(char* name, Storage* storage) {
    UNUSED(storage);
    uint8_t sk[32];
    furi_hal_random_fill_buf(sk, 32);

    uint8_t pk[32];
    crypto_x25519_public_key(pk, sk);

    FuriString* sk_filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
    FuriString* pk_filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    furi_string_cat_str(sk_filepath, "/");
    furi_string_cat_str(
        sk_filepath, name); // will be = to DATA PATH + name, so perfect time to make folder
    storage_common_mkdir(storage, furi_string_get_cstr(sk_filepath));
    furi_string_cat_str(sk_filepath, "/x25519");

    furi_string_cat_str(pk_filepath, "/");
    furi_string_cat_str(pk_filepath, name);
    furi_string_cat_str(pk_filepath, "/x25519_pub");

    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, furi_string_get_cstr(sk_filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        return false;
    }

    if(!storage_file_write(file, sk, 32)) {
        return false;
    }

    storage_file_close(file);
    storage_file_free(file);

    File* pub_file = storage_file_alloc(storage);

    if(!storage_file_open(
           pub_file, furi_string_get_cstr(pk_filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        return false;
    }

    if(!storage_file_write(pub_file, pk, 32)) {
        return false;
    }

    furi_string_free(sk_filepath);
    furi_string_free(pk_filepath);

    storage_file_close(pub_file);
    storage_file_free(pub_file);

    return true;
}

Identity keyer_generate_keypair() {
    uint8_t sk[32];
    furi_hal_random_fill_buf(sk, 32);
    uint8_t pk[32];
    crypto_x25519_public_key(pk, sk);
    return (Identity){.public_key = pk, .secret_key = sk};
}

bool keyer_save_identity(Identity* idn, Storage* storage, char* name) {
    if(idn == NULL) {
        return false;
    }

    FuriString* filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
    furi_string_cat_str(filepath, "/");
    furi_string_cat_str(filepath, name);
    if(!storage_common_exists(storage, furi_string_get_cstr(filepath))) {
        storage_common_mkdir(storage, furi_string_get_cstr(filepath));
    }

    furi_string_cat_str(filepath, "/x25519");

    if(idn->secret_key != NULL) {
        File* file = storage_file_alloc(storage);
        if(!storage_file_open(file, furi_string_get_cstr(filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS) ||
           !storage_file_write(file, idn->secret_key, 32)) {
            return false;
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    // does fd need to be freed?

    if(idn->public_key != NULL) {
        furi_string_cat_str(filepath, "_pub");
        File* file = storage_file_alloc(storage);
        if(!storage_file_open(file, furi_string_get_cstr(filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS) ||
           !storage_file_write(file, idn->public_key, 32)) {
            return false;
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    return true;
}