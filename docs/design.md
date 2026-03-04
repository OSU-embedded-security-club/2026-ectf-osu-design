# High Level Design

@plantumlfile states.pu

### Pin management

Problem statement:
* Assume that any user input at any time could cause an arbitrary memory read
    * Meaning the moment uart is open, assume any clear text secrets are vulnerable
* They keystore is out of scope, no truly secure places exist to hide data
* The timing requirements, combined with the slow CPU, mean that any hash we use is bound to be insecure
* The pin is defined by 6 hexadecimal digits, or 16,777,216 combinations
    * This is small enough simply divulging an easily computable hash is akin to the pin itself being leaked



Proposed mitigations:
* Hashing should be done with BLAKE2b loops, easily tuned for security versus effort
* Double hash system:
    * First phase is PIN + key only, bulk of the hash work
        * The key in the generate_secrets.py. 
    * Second phase is done on device, after boot with much device specific data. 
        * Data included:
            * Use the BOOTCRC from FACTORYREGION, this is a crc of all the device specific values, uuids and calibration ids.
            * A second IV generated at boot with the TRNG module
            * More random data generated in generate_secrets.py, also randomly put in the code segment via C macros
        * This will not be done with multiple iterations, and instead rely on high input amounts and low data continuity. 
    * In general, the hashing input must be spread throughout ram, requirng attacks to use multiple arbitrary read attacks to obtain

## Groups

Every group has 3 permissions and each permission has a corresponding asymetric key pair. This means every group has 3 different key pairs. An HSM has the permission for a group if it is provisioned with the private key for a permission in a group. **All HSMs are provisioned with all public keys.** This is important to remember for later.

@plantumlfile group.pu


## File Storage

Files are stored using the [file_slot_entry_t struct](@ref file_slot_entry_t).

### File Encryption
When a file is uploaded to the board a random X25519 Key Pair is generated from the TRNG. A X25519 Key Exchange done with this randomly generated key and the public key for the file's group. This generated secret is then used to encrypt the file using AES-GCM. After the file is encrypted the private portion of the key pair and the generated secret key are erased from memory. The public key of the randomly generated key pair is then stored in the metadata for the file. Since the randomly generated private key has been erased the only way the AES secret can be regenerated is using the file groups read private key and the public key stored in the files metadata. The file should only be able to be decrypted by HSMs that have the read permission for the file's group (Only HSMs with the read permission have the read private key).

**NOTE: Anyone can encrypt the file using the read public key so we must perform verification using another method.**

### File Verification
Files are only supposed to be written by HSMs that have the write permission for a group (and the coresponding write private key). To verify that the file was written by a valid writer we can sign the files metadata using ED25519. Any HSM can verify the metadata using the write public key for the coresponding group.

## File Transfer
When a transfer is initated the sender starts by generating a random X25519 Key Pair. The public key of the randomly generated pair should then be sent to receiving HSM. After generating the random key pair a secret should be generated using the randomly generated private key and the group's transfer public key that the sender has stored internally. The entire [file_slot_entry_t struct](@ref file_slot_entry_t) should be encrypted using the generated secret and sent over UART. If the receiving HSM has the transfer private key / transfer permission they will be able to regenerate the secret using the transfer private key and the public key that was provided via UART.

@plantumlfile file_transfer.pu

**NOTE: This is a very high level overview, some metadata will have to be exchanged.**