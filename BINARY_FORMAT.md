A encrypted message, which we will call a `EncryptedMessage`, will be formated like so:

<nonce (24 bytes)><cipher (x bytes)><mac (16 bytes)>


A anonymous encrypted message, which we will call a `AnonymousEncryptedMessage`, will be formated like so:

<ephemoric public key (32 bytes)><EncryptedMessageMessage (y bytes)>


Any message will be formatted like so. Max size is 64 bytes as thats what CC1101 can handle so multiple
of these may be required for a single message.

"KY"<version counter (1 byte)><message type id (1 byte)><portion of message in rest of buffer><Message (p bytes)>


Message type 9: is a continuation

