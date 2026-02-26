# TODO: Put in a more competition ready format

import argparse
import pathlib
import json

import os, struct, hashlib, time
from os.path import join, dirname, abspath
import jinja2

SOURCE_DIR = dirname(abspath(__file__))
SECRETS_BASE = join(SOURCE_DIR, "include", "secrets.h.base")
SECRETS_H_OUT = join(SOURCE_DIR, "include", "secrets.h")
SECRETS_C_OUT = join(SOURCE_DIR, "src", "secrets.c")

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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser("Secrets Generator")

    parser.add_argument(
        "secrets",
        type=pathlib.Path,
        help="Path to secrets file"
    )

    parser.add_argument(
        "hsm_pin",
        type=str,
        help="User PIN for the HSM"
    )

    parser.add_argument(
        "permissions",
        type=str,
        help="List of colon-separated permissions. E.g., \"1234=R--:4321=RWC\""
    )

    return parser.parse_args()

def main():

    args = parse_args()

    # Load Secrets
    secrets: dict
    with open(args.secrets) as reader:
        secrets = json.load(reader)

    # Create HSM Pin Hash

    # Create HSM Permissions
    groups = dict()
    for permission_str in args.permissions.split(':'):
        group_id: str
        permissions: str
        group_id, permissions = permission_str.split('=')
        
        groups[group_id] = {
            "read": {},
            "write": {},
            "transfer": {}
        }
        groups[group_id]["read"]["private"] = secrets[group_id]["read"]["private"] if permissions[0] == 'R' else None
        groups[group_id]["write"]["private"] = secrets[group_id]["write"]["private"] if permissions[1] == 'W' else None
        groups[group_id]["transfer"]["private"] = secrets[group_id]["transfer"]["private"] if permissions[2] == "C" else None

        groups[group_id]["read"]["public"] = secrets[group_id]["read"]["public"]
        groups[group_id]["write"]["public"] = secrets[group_id]["write"]["public"]
        groups[group_id]["transfer"]["public"] = secrets[group_id]["transfer"]["public"]

    jinja_env = jinja2.Environment(loader=jinja2.FileSystemLoader("."))
    
    jinja_h_file = jinja_env.get_template("include/secrets.h.j2")
    jinja_c_file = jinja_env.get_template("src/secrets.c.j2")

    with open("include/secrets.h", 'w') as out:
        out.write(jinja_h_file.render(
            STAGE1_PIN_ITERATIONS=STAGE1_PIN_ITERATIONS,
            STAGE1_KEY=STAGE1_KEY.hex(),
            STAGE1_PADDING=STAGE1_PADDING.hex(),
            STAGE2_RAND_INT=STAGE2_RAND_INT,
            STAGE1_PIN_HASH=hash_pin(args.hsm_pin).hex(),
            groups=groups
        ))

    with open("src/secrets.c", 'w') as out:
        out.write(jinja_c_file.render(
            groups = groups,
            transfer_key = secrets["transfer_key"]
        ))

    # base = open(SECRETS_BASE).read()

    # f = open(SECRETS_OUT, "w")
    # f.write(base)
    # f.write(f"\n#define STAGE1_PIN_ITERATIONS {STAGE1_PIN_ITERATIONS}")
    # f.write(f"\n#define STAGE1_KEY {make_c_bytes(STAGE1_KEY)}")
    # f.write(f"\n#define STAGE1_PADDING {make_c_bytes(STAGE1_PADDING)}")
    # f.write(f"\n#define STAGE2_RAND_INT {STAGE2_RAND_INT}U")
    # f.write(f"\n\n#define STAGE1_PIN_HASH {make_c_bytes(hash_pin(PIN))}")


if __name__ == "__main__":
    main()
