# TODO: Put in a more competition ready format

import os, struct, hashlib, time
from os.path import join, dirname, abspath

SOURCE_DIR = dirname(abspath(__file__))
SECRETS_BASE = join(SOURCE_DIR, "include", "secrets.h.base")
SECRETS_OUT = join(SOURCE_DIR, "include", "secrets.h")

# Contants

# **THIS IS WHERE YOU TUNE THE HASHING TIME**:

# 64 byte blake2b = 155503 cycles, or 0.00485946875 seconds, or 205.78hz
# Here, we are targeting 250 ms for the STAGE1 hash.
# Please note, this is a hilariously weak hash, but it is the best we can do.
STAGE1_PIN_ITERATIONS = int((205 / 1000) * 250)


STAGE1_KEY = os.urandom(32)
STAGE1_PADDING = os.urandom(64 - 6)

STAGE2_RAND_INT = struct.unpack("<I", os.urandom(4))[0]

# Todo: change this
PIN = "aBcDe6"


def hash_pin(pin):
    assert len(pin) == 6
    # Throws an error if invalid
    int(pin, 16)

    hash = pin.lower().encode("ascii") + STAGE1_PADDING
    for _ in range(STAGE1_PIN_ITERATIONS):
        hash = hashlib.blake2b(hash, key=STAGE1_KEY).digest()
    return hash


def make_c_bytes(data):
    return "{" + ", ".join(f"0x{b:02x}" for b in data) + "}"


def main():
    base = open(SECRETS_BASE).read()

    f = open(SECRETS_OUT, "w")
    f.write(base)
    f.write(f"\n#define STAGE1_PIN_ITERATIONS {STAGE1_PIN_ITERATIONS}")
    f.write(f"\n#define STAGE1_KEY {make_c_bytes(STAGE1_KEY)}")
    f.write(f"\n#define STAGE1_PADDING {make_c_bytes(STAGE1_PADDING)}")
    f.write(f"\n#define STAGE2_RAND_INT {STAGE2_RAND_INT}U")
    f.write(f"\n\n#define STAGE1_PIN_HASH {make_c_bytes(hash_pin(PIN))}")


if __name__ == "__main__":
    main()
