#include <furi.h>
#include <keyer.h>

// make union types for all boxes
// all these box should be "private fields"

typedef struct NormalBox {
    Identity encrypt_as;
    Identity encrypt_for;
    uint8_t* input_message;
    size_t input_message_size;
    uint8_t* output_message;
    size_t output_message_size;
} NormalBox;

NormalBox normalbox_init() {
    return (NormalBox){
        .encrypt_as = keyer_identity_init(NULL, NULL),
        .encrypt_for = keyer_identity_init(NULL, NULL),
        .input_message = NULL,
        .output_message = NULL,
    };
}

void normalbox_set_identity_target(NormalBox* box, Identity idn) {
    box->encrypt_for = idn;
}

void normalbox_set_identity_signer(NormalBox* box, Identity idn) {
    box->encrypt_as = idn;
}

void normalbox_set_message_string(NormalBox* box, char* given_message) {
    //input edit
    size_t size = str_len(given_message);
    memcpy(box->input_message, given_message, size);
    box->input_message_size = size;
}
void normalbox_encrypt(NormalBox* box) {
    size_t size = box->input_message_size + 24 + 16;
    box->output_message = malloc(size);
    box->output_message_size = size;
    // encrypt

    free(box->input_message);
}

void normalbox_decrypt(NormalBox* box) {
    size_t size = box->input_message_size - 24 - 16;
    box->output_message = malloc(size);
    box->output_message_size = size;
    // decrypt
    free(box->input_message);
}

typedef struct AnonBox {
    Identity encrypt_for;
    Identity ephemeral_encrypt_as;
    uint8_t* input_message;
    size_t input_message_size;
    uint8_t* output_message;
    size_t output_message_size;
} AnonBox;

AnonBox anonbox_init() {
    Identity ephemeral = keyer_generate_keypair();
    Identity temp_init_identity = keyer_identity_init(NULL, NULL);

    return (AnonBox){
        .encrypt_for = temp_init_identity,
        .ephemeral_encrypt_as = ephemeral,
        .input_message = NULL,
        .input_message_size = 0,
        .output_message = NULL,
        .output_message_size = 0,
    };
}

void anonbox_set_identity_target(AnonBox* box, Identity idn) {
    box->encrypt_for = idn;
}

void anonbox_set_message_string(AnonBox* box, char* given_message) {
    size_t size = str_len(given_message);
    memcpy(box->input_message, given_message, size);
    box->input_message_size = size;
}
void anonbox_encrypt(AnonBox* box) {
    size_t size = box->input_message_size + 24 + 16 + 32;
    box->output_message = malloc(size);
    box->output_message_size = size;
    // encrypt

    free(box->input_message);
}

void anonbox_decrypt(AnonBox* box) {
    size_t size = box->input_message_size - 24 - 16 - 32;
    box->output_message = malloc(size);
    box->output_message_size = size;
    // decrypt
    free(box->input_message);
}
