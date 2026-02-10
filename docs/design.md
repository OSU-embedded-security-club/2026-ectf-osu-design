# Our Design








### Pin management
**TODO:** Some of this could me moved to a more general section 


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
    










