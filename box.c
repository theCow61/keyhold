#include <furi.h>
#include <keyer.h>

// make union types for all boxes

typedef struct NormalBox {
    Identity encrypt_as;
    Identity encrypt_for;
    uint8_t* message;
    size_t message_size;
} NormalBox;

NormalBox* normal_box_alloc() {
    NormalBox* box = malloc(sizeof(NormalBox));
    box->encrypt_as = keyer_identity_init(NULL, NULL);
    box->encrypt_for = keyer_identity_init(NULL, NULL);
    box->message = NULL;
    box->message_size = 0;
    return box;
}

void normal_box_set_plain(NormalBox* box, char* buf) {
}
