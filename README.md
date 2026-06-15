# keyhold


Live examples and commentary can be found [here](https://salti.tech/posts/keyhold).

Keyhold is a cryptographic key manager+ on your flipper zero device. You may generate keys and encrypt messages for other people using asymmetric encryption with x25519. Encrypted messages can be sent over the
sub ghz device on the flipper zero. Messages may also be decrypted.

Bluetooth security is also a feature as keeping your private keys stored on a SD card may be risky due to theft. So, you can
encrypt all of your private keys (via ChaCha20) and the encryption key will be on the serial characterstic over bluetooth. If you decide to encrypt your private keys,
you will need to authenicate over bluetooth (on the
serial characterstic) using the encryption key for everytime you turn on the device. Authenticating should decrypt your private keys and store the decrypted keys in memory, and should stay in memory even
if you leave the application; this is so you only need to authenticate when you turn on the device, which shouldn't be often considering that the flipper zero can stay on for a very long time.
Ofcourse, this is still vulnerable because the private keys are easily visable in memory, but this is still better than keeping them raw on storage.

# Project Plan

* Generate key pairs as "identities" 🟢
* Save identities like contacts 🟢
* Identity sharing
    * NFC tap to do key exchange ⚪
    * Same ways of import/export of encrypted messages, just needs to be added to UI.
* Import/Export features of encrypted messages
    * Save as file 🟢
    * Import file 🟢
    * Send over radio 🟢
    * Receive over radio ⚪
* Message encryption
    * file encryption 🟢
    * text encryption 🟢
* Encryption options
    * Anonymous encryption with ephemeral public key 🟡
    * Encrypter contact/identity needs to be known for decryption 🟢
* Security
    * TPM Chip ⚪
    * Keys on storage encrypted and restored via bluetooth with phone 🟢
* Binary schema for data
    * Protobuf ⭕
    * Own implementation 🟢
* Major refactor of code base

