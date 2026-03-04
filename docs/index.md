# Overview

The goal for our design was to use asymetric encryption to ensure that secrets to read, write, and transfer files are only provisioned on HSMs with valid permissions to perform the operation. We used monocypher's implementation of ED25519 and EdDSA to perform our asymetric cryptographic operations and the MSPM0l2228's hardware AES for accelerated encryption and decryption.

# Contents

- [High Level Design](design.md)
- [Miscellaneous Implementation Details](misc.md)
- [Read Command](@ref message_read_response)
- [Write Command](@ref message_write_response)
- [List Command](@ref message_list_response)
- [Interrogate Command](@ref message_interrogate)
- [Recieve Command](@ref message_recieve)
- [Listen Command](@ref message_listen)