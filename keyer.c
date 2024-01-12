#include "keyer.h"
#include "core/record.h"
#include "lib/monocypher/monocypher.h"
#include "storage/filesystem_api_defines.h"
#include <furi.h>
#include <furi_hal_random.h>
#include <storage/storage.h>
// #include <toolbox/buffer_stream.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

Identity keyer_identity_init(uint8_t* sk, uint8_t* pk) {
    // Identity* identity = malloc(sizeof(Identity));
    // identity->public_key = pk;
    // identity->secret_key = sk;
    // return identity;
    return (Identity){.public_key = pk, .secret_key = sk};
}

// void keyer_identity_free(Identity* identity) {
//     keyer_identity_clear(identity);
//     free(identity);
// }

void keyer_identity_clear(Identity* idn) {
    if(idn == NULL) return;
    if(idn->secret_key != NULL) {
        crypto_wipe(idn->secret_key, 32);
        free(idn->secret_key);
    }
    if(idn->public_key != NULL) {
        crypto_wipe(idn->public_key, 32);
        free(idn->public_key);
    }
    idn->secret_key = NULL;
    idn->public_key = NULL;
}

// bool keyer_generate_key_pair_and_save(char* name, Storage* storage) {
//     UNUSED(storage);
//     uint8_t sk[32];
//     furi_hal_random_fill_buf(sk, 32);

//     uint8_t pk[32];
//     crypto_x25519_public_key(pk, sk);

//     FuriString* sk_filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
//     FuriString* pk_filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

//     furi_string_cat_str(sk_filepath, "/");
//     furi_string_cat_str(
//         sk_filepath, name); // will be = to DATA PATH + name, so perfect time to make folder
//     storage_common_mkdir(storage, furi_string_get_cstr(sk_filepath));
//     furi_string_cat_str(sk_filepath, "/x25519");

//     furi_string_cat_str(pk_filepath, "/");
//     furi_string_cat_str(pk_filepath, name);
//     furi_string_cat_str(pk_filepath, "/x25519_pub");

//     File* file = storage_file_alloc(storage);

//     if(!storage_file_open(file, furi_string_get_cstr(sk_filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
//         return false;
//     }

//     if(!storage_file_write(file, sk, 32)) {
//         return false;
//     }

//     storage_file_close(file);
//     storage_file_free(file);

//     File* pub_file = storage_file_alloc(storage);

//     if(!storage_file_open(
//            pub_file, furi_string_get_cstr(pk_filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
//         return false;
//     }

//     if(!storage_file_write(pub_file, pk, 32)) {
//         return false;
//     }

//     furi_string_free(sk_filepath);
//     furi_string_free(pk_filepath);

//     storage_file_close(pub_file);
//     storage_file_free(pub_file);

//     return true;
// }

Identity keyer_generate_keypair() {
    // uint8_t sk[32];
    uint8_t* sk = malloc(32 * sizeof(uint8_t));
    furi_hal_random_fill_buf(sk, 32);
    uint8_t* pk = malloc(32 * sizeof(uint8_t));
    // uint8_t pk[32];
    crypto_x25519_public_key(pk, sk);
    return keyer_identity_init(sk, pk);
    // return (Identity){.public_key = pk, .secret_key = sk};
}

// size_t string_to_bbuffer(uint8_t* buffer, char* str) {
//     int loop;
//     size_t i;

//     loop = 0;
//     i = 0;

//     while(str[loop] != '\0') {
//         buffer[i++] = str[loop++];
//     }
//     return i;
// }

size_t str_len(char* str) {
    size_t i;
    i = 0;
    while(str[i] != '\0') {
        i++;
    }
    return i;
}

bool keyer_save_identity(Identity* idn, Saves* saves, Storage* storage, char* name) {
    if(idn == NULL) {
        return false;
    }

    FuriString* filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
    furi_string_cat_str(filepath, "/");
    furi_string_cat_str(filepath, name);
    if(!storage_common_exists(storage, furi_string_get_cstr(filepath))) {
        storage_common_mkdir(storage, furi_string_get_cstr(filepath));

        // add to save memory

        saves_add(saves, name);

        // add to savee file

        File* conf_file = storage_file_alloc(storage);

        size_t o_name_len = str_len(name);
        name[o_name_len] = '\n';
        name[o_name_len + 1] = '\0'; // I don't need this as im just copying the bytes before it ?

        if(!storage_file_open(conf_file, SAVED_FILE, FSAM_WRITE, FSOM_OPEN_APPEND) ||
           !storage_file_write(conf_file, name, (o_name_len + 1) * sizeof(char))) {
            return false;
        }

        name[o_name_len] = '\0';

        storage_file_close(conf_file);
        storage_file_free(conf_file);
    }

    furi_string_cat_str(filepath, "/x25519");

    if(idn->secret_key != NULL) {
        File* file = storage_file_alloc(storage);
        if(!storage_file_open(file, furi_string_get_cstr(filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS) ||
           !storage_file_write(file, idn->secret_key, 32)) {
            furi_string_free(filepath);
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
            furi_string_free(filepath);
            return false;
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_string_free(filepath);

    return true;
}

Identity keyer_get_pub_identity(Storage* storage, const char* name) {
    FuriString* filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
    furi_string_cat_str(filepath, "/");
    furi_string_cat_str(filepath, name);
    furi_string_cat_str(filepath, "/x25519_pub");

    Identity idn = keyer_identity_init(NULL, NULL);

    if(storage_file_exists(storage, furi_string_get_cstr(filepath))) { // pub key

        File* pk_file = storage_file_alloc(storage);
        if(storage_file_open(
               pk_file, furi_string_get_cstr(filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint8_t* pk_buf = malloc(32 * sizeof(uint8_t));

            if(!storage_file_read(pk_file, pk_buf, 32)) {
                free(pk_buf);
                pk_buf = NULL;
            }

            idn.public_key = pk_buf;

            storage_file_close(pk_file);
        }
        storage_file_free(pk_file);
    }

    furi_string_free(filepath);
    return idn;
}

Identity keyer_get_identity(Storage* storage, const char* name) {
    // FuriString* filepath = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);
    FuriString* filepath = furi_string_alloc_set("/any/apps_data/keyhold/"); // bizare. the /data path only sometimes works. Fix this as this path may be obsoluted.
    // furi_string_cat_str(filepath, "/");
    furi_string_cat_str(filepath, name);
    furi_string_cat_str(filepath, "/x25519");
    FURI_LOG_D("keyhold", "%s", furi_string_get_cstr(filepath));

    Identity idn = keyer_identity_init(NULL, NULL);

    FURI_LOG_D("keyhold", "bruh0");
    if(storage_file_exists(storage, furi_string_get_cstr(filepath))) { // sec key
        FURI_LOG_D("keyhold", "bruh1");
        File* sk_file = storage_file_alloc(storage);
        if(storage_file_open(
               sk_file, furi_string_get_cstr(filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint8_t* sk_buf = malloc(32 * sizeof(uint8_t));

            FURI_LOG_D("keyhold", "bruh2");

            if(!storage_file_read(sk_file, sk_buf, 32)) {
                free(sk_buf);
                sk_buf = NULL;
            }

            idn.secret_key = sk_buf;

            storage_file_close(sk_file);
        }
        storage_file_free(sk_file);
    }


    furi_string_cat_str(filepath, "_pub");

    // Duplicated code to prevent extra file existance check

    if(storage_file_exists(storage, furi_string_get_cstr(filepath))) { // pub key
        File* pk_file = storage_file_alloc(storage);
        if(storage_file_open(
               pk_file, furi_string_get_cstr(filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint8_t* pk_buf = malloc(32 * sizeof(uint8_t));

            if(!storage_file_read(pk_file, pk_buf, 32)) {
                free(pk_buf);
                pk_buf = NULL;
            }

            idn.public_key = pk_buf;

            storage_file_close(pk_file);
        }
        storage_file_free(pk_file);
    }

    furi_string_free(filepath);
    return idn;
}

typedef struct EncryptorConfig {
    Identity encrypt_as;
    Identity encrypt_for;
    uint8_t* plain_bytes;
    /**
     * encrypt_bytes is more like nonce + encrypted bytes + mac . size is size + sizeof(nonce) + sizeof(mac)
    */
    uint8_t* encrypt_bytes;
    size_t size; // size of cipher/encrypted parts of encrypt_bytes in "length" like [size]
} EncryptorConfig;

EncryptorConfig* encryptor_config_alloc() {
    EncryptorConfig* ecf = malloc(sizeof(EncryptorConfig));
    ecf->encrypt_as = keyer_identity_init(NULL, NULL);
    ecf->encrypt_for = keyer_identity_init(NULL, NULL);
    ecf->plain_bytes = NULL;
    ecf->encrypt_bytes = NULL;
    ecf->size = 0;
    return ecf;
}

// make one for non str, MUST GO THROUGH THIS FOR THE MALLOC AND SIZE SET AND BUFFER SYNC
void encryptor_config_sync_pbuffer_str(EncryptorConfig* ecf, char* buf) {
    ecf->plain_bytes = (uint8_t*)strdup(buf); // SIGNED TO UNSIGNED CAST
    ecf->size = str_len((char*)ecf->plain_bytes);
    ecf->encrypt_bytes = malloc((ecf->size + 24 + 16) * sizeof(uint8_t));
}

void encryptor_config_reset(EncryptorConfig* ecf) {
    if(ecf == NULL) return;
    if(ecf->plain_bytes != NULL) {
        crypto_wipe(ecf->plain_bytes, ecf->size);
        free(ecf->plain_bytes);
    }
    if(ecf->encrypt_bytes != NULL) {
        crypto_wipe(ecf->encrypt_bytes, ecf->size + 24 + 16);
        free(ecf->encrypt_bytes);
    }
    ecf->plain_bytes = NULL;
    ecf->encrypt_bytes = NULL;
    // keyer_identity_clear(&ecf->encrypt_as);
    // keyer_identity_clear(&ecf->encrypt_for);
    ecf->size = 0;
}

void encryptor_config_free(EncryptorConfig* ecf) {
    encryptor_config_reset(ecf);
    free(ecf);
}

uint8_t* encryptor_config_get_plain_buffer(EncryptorConfig* ecf) {
    return ecf->plain_bytes;
}

void encryptor_config_set_as_identity(EncryptorConfig* ecf, Identity idn) {
    ecf->encrypt_as = idn;
}
void encryptor_config_set_to_identity(EncryptorConfig* ecf, Identity idn) {
    ecf->encrypt_for = idn;
}

uint8_t* encryptor_config_encrypt(EncryptorConfig* ecf) {
    uint8_t* from_sk = ecf->encrypt_as.secret_key;
    // uint8_t* from_pk = ecf->encrypt_as.public_key;
    uint8_t* to_pk = ecf->encrypt_for.public_key;

    size_t size = ecf->size;

    uint8_t shared_secret[32];

    crypto_x25519(shared_secret, from_sk, to_pk);

    uint8_t shared_key[32];

    crypto_chacha20_h(shared_key, shared_secret, (uint8_t[16]){0});

    // encrypt-bytes is passed in as nonce as first 24 bytes is nonce

    uint8_t* nonce = ecf->encrypt_bytes; // first 24 bytes
    furi_hal_random_fill_buf(ecf->encrypt_bytes, 24); //nonce
    uint8_t* enc_bytes = ecf->encrypt_bytes + 24; // between first and last 24 and 16 bytes
    uint8_t* mac = ecf->encrypt_bytes + 24 + size; // last 16 bytes

    crypto_aead_lock(
        enc_bytes, // encryption part
        mac, // mac
        shared_key,
        nonce, // nonce
        NULL,
        0,
        ecf->plain_bytes,
        size);

    // nonce cipher mac

    // memcpy(ecf->encrypt_bytes, nonce, 24);
    // memcpy(ecf->encrypt_bytes + 24, enc_bytes, size);
    // memcpy(ecf->encrypt_bytes + 24 + size, mac, 16);

    // nonce does not need to be wiped, let it fall off stack
    // enc_bytes does not need to be wiped, let it fall off stack
    // mac might need to be wiped

    crypto_wipe(ecf->plain_bytes, size);
    ecf->plain_bytes = NULL;
    crypto_wipe(shared_secret, 32);
    crypto_wipe(shared_key, 32);

    // keyer_identity_clear(&ecf->encrypt_as);
    // keyer_identity_clear(&ecf->encrypt_for);

    return ecf->encrypt_bytes;
}

size_t encryptor_config_get_size(EncryptorConfig* ecf) {
    return ecf->size;
}

/** 
 * sets to decryption mode. initializes the buffers and returns the encryption buffer. updates size.
*/
uint8_t* encryptor_config_encrypted_bytes_buf_init(EncryptorConfig* ecf, size_t size) {
    ecf->encrypt_bytes = malloc(size); // might have to be * sizeof(uint8_t)
    ecf->plain_bytes = malloc(size);
    ecf->size = size - (24 + 16);
    return ecf->encrypt_bytes;
}

/**
 * Size should be full message size, after this it will just be size of deciphered part
*/
uint8_t* encryptor_config_decrypt(EncryptorConfig* ecf) {
    // size should be of whole thing (nonce mac included)

    uint8_t* sender_pk =
        ecf->encrypt_as.public_key; // keep in mind for future the anonymous feature

    uint8_t* our_sk = ecf->encrypt_for.secret_key;

    uint8_t shared_secret[32];

    crypto_x25519(shared_secret, our_sk, sender_pk);

    uint8_t shared_key[32];

    crypto_chacha20_h(shared_key, shared_secret, (uint8_t[16]){0});

    if(crypto_aead_unlock(
           ecf->plain_bytes,
           ecf->encrypt_bytes + ecf->size + 24,
           shared_key,
           ecf->encrypt_bytes,
           NULL,
           0,
           ecf->encrypt_bytes + 24,
           ecf->size)) {
        crypto_wipe(shared_secret, 32);
        crypto_wipe(shared_key, 32);
        return NULL;
    }

    crypto_wipe(shared_secret, 32);
    crypto_wipe(shared_key, 32);
    // keyer_identity_clear(&ecf->encrypt_for); // this could be a pointer to encrypted keys.
    // keyer_identity_clear(&ecf->encrypt_as);
    free(ecf->encrypt_bytes);
    ecf->encrypt_bytes = NULL;
    return ecf->plain_bytes;
}


void keyer_encrypt_x25509_key_file_chacha20(Storage* storage, uint8_t* encryptor_key, const char* save_name, uint8_t* save_sk_key) {

    // uint8_t nonce[24] = {0}; // LOOK INTO THIS TODO REMEMBER. IS SAME NONCE OK FOR SAME MESSAGE?
    // furi_hal_random_fill_buf(nonce, 24);
    
    uint8_t* nonce = encryptor_key; // overlapping nonce with encryptor_key, should use first 24 bytes.

    uint8_t ciphered_sk_key[32];

    crypto_chacha20_x(ciphered_sk_key, save_sk_key, 32, encryptor_key, nonce, 0);

    FuriString* filepath = furi_string_alloc_set("/any/apps_data/keyhold/");
    // furi_string_cat_str(filepath, "/");
    furi_string_cat_str(filepath, save_name);
    furi_string_cat_str(filepath, "/x25519");

    File* sk_file = storage_file_alloc(storage);

    storage_file_open(sk_file, furi_string_get_cstr(filepath), FSAM_WRITE, FSOM_OPEN_ALWAYS);
    storage_file_write(sk_file, ciphered_sk_key, 32);
    storage_file_close(sk_file);
    storage_file_free(sk_file);

    furi_string_free(filepath);

}


/**
* Checks if we have our private key in our record containing plain text private keys.
* If we don't, then we use the normal file reading method.
* This should not be used for Identities who we are non owned (no private key), or we don't need a private key.
* If this is the case, then use keyer_get_pub_identity.
* 
* size_t idx: index associated with the name of the identity.
*/
Identity keyer_get_correct_identity(Storage* storage, uint8_t** encrypted_keys, const char* name, size_t idx) {
    if (encrypted_keys == NULL || encrypted_keys[idx] == NULL) {
        return keyer_get_identity(storage, name);
    }
    
    Identity toReturn = keyer_get_pub_identity(storage, name);
    toReturn.secret_key = encrypted_keys[idx];

    return toReturn;
}











