#include <furi.h>
#include <furi_hal_random.h>
#include <pb_common.h>
#include <pb_encode.h>
#include <keyhold.pb.h>
#include "encrypter.h"
#include "keyer.h"
#include "lib/monocypher/monocypher.h"

struct Cipher {
    size_t len;
    uint8_t* cipher;
};

typedef struct _Encrypter {
    uint8_t* plain_text;
    size_t size_plain_text;
    Identity encrypt_as;
    Identity encrypt_for;
    uint8_t* encoded_message;
} Encrypter;

static bool bytes_callback(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
    struct Cipher* ciph = *arg;
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, ciph->cipher, ciph->len);
}

uint8_t* encrypter_encrypt_message(Encrypter* enc) {
    uint8_t* sk_ours = enc->encrypt_as.secret_key;
    uint8_t* pk_theirs = enc->encrypt_for.public_key;

    uint8_t shared_secret[32];
    crypto_x25519(shared_secret, sk_ours, pk_theirs);

    uint8_t shared_key[32];
    crypto_chacha20_h(shared_key, shared_secret, (uint8_t[16]){0});

    uint8_t cipher[enc->size_plain_text];

    PB_Keyhold_Message message = PB_Keyhold_Message_init_default;
    furi_hal_random_fill_buf(message.nonce.bytes, message.nonce.size);

    crypto_aead_lock(
        cipher,
        message.mac.bytes,
        shared_key,
        message.nonce.bytes,
        NULL,
        0,
        enc->plain_text,
        enc->size_plain_text);

    // for dynamic bytes

    // pb_ostream_t sizestream = {0};
    // pb_encode(&sizestream, PB_Keyhold_Message_fields, &message);
    // pb_encode_tag(&sizestream, PB_WT_STRING, PB_Keyhold_Message_cipher_tag);
    // pb_encode_string(&sizestream, cipher, enc->size_plain_text);
    // size_t encoded_message_size = sizestream.bytes_written;
    // so on

    // same as above but instead uses callback
    struct Cipher mycipher = {.len = enc->size_plain_text, .cipher = cipher};
    message.cipher.funcs.encode = bytes_callback;
    message.cipher.arg = &mycipher;

    // get size
    pb_ostream_t sizestream = {0};
    pb_encode(&sizestream, PB_Keyhold_Message_fields, &message);
    size_t encoded_message_size = sizestream.bytes_written;

    enc->encoded_message = malloc(encoded_message_size);
    pb_ostream_t stream = pb_ostream_from_buffer(enc->encoded_message, encoded_message_size);
    pb_encode(&stream, PB_Keyhold_Message_fields, &message);

    crypto_wipe(enc->plain_text, enc->size_plain_text);
    crypto_wipe(shared_secret, 32);
    crypto_wipe(shared_key, 32);
    keyer_identity_clear(&enc->encrypt_as);
    keyer_identity_clear(&enc->encrypt_for);
    free(enc->plain_text);
    enc->plain_text = NULL;

    return enc->encoded_message;
}
