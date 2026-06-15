# keyhold
Flipper Zero application allowing for key exchange between devices over multiple protocols and encryption/decryption.

Live examples and further commentary can be found [here](https://salti.tech/posts/keyhold).

Keyhold is a cryptographic key manager+ on your flipper zero device. You may generate keys and encrypt messages for other people using asymmetric encryption with x25519. Encrypted messages can be sent over the
sub ghz device on the flipper zero. Messages may also be decrypted.

Bluetooth security is also a feature as keeping your private keys stored on a SD card may be risky due to theft. So, you can
encrypt all of your private keys (via ChaCha20) and the encryption key will be on the serial characterstic over bluetooth. If you decide to encrypt your private keys,
you will need to authenicate over bluetooth (on the
serial characterstic) using the encryption key for everytime you turn on the device. Authenticating should decrypt your private keys and store the decrypted keys in memory, and should stay in memory even
if you leave the application; this is so you only need to authenticate when you turn on the device, which shouldn't be often considering that the flipper zero can stay on for a very long time.
Ofcourse, this is still vulnerable because the private keys are easily visable in memory, but this is still better than keeping them raw on storage.

