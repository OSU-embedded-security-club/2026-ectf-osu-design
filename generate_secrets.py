from simple_config_writer import ConfigFile, CVar
from simple_crypto import RNG, Hasher

# 64 byte blake2b = 155503 cycles, or 0.00485946875 seconds, or 205.78hz
# Here, we are targeting 250 ms for the STAGE1 hash.
# Please note, this is a hilariously weak hash, but it is the best we can do.
STAGE1_PIN_ITERATIONS: int = int((205 / 1000) * 250)


STAGE1_KEY: bytes = RNG.rand(64)
STAGE1_PADDING: bytes = RNG.rand(64 - 6)
STAGE2_RAND_INT: int = int.from_bytes(RNG.rand(4), "big")
ROOT_KEY: bytes = RNG.rand(32)

# Todo: change this
PIN = "aBcDe6"


def hash_pin(pin: str) -> bytes:
    """
    Hash the pin using the stage 1 parameters.

    Args:
        pin (str): The pin to hash, which must be a 6 character hexadecimal string.

    Returns:
        bytes: The hash of the pin, which will be stored in the config file.
    """

    assert len(pin) == 6
    assert all(c in "0123456789abcdefABCDEF" for c in pin)

    hash: bytes = Hasher.hash(
        STAGE1_PIN_ITERATIONS,
        STAGE1_KEY,
        pin.lower().encode("ascii") + STAGE1_PADDING,
    )

    return hash


def main() -> None:
    config: ConfigFile = ConfigFile()

    pin_iters: CVar = CVar(
        "static const int",
        "STAGE1_PIN_ITERATIONS",
        STAGE1_PIN_ITERATIONS,
    )
    config.add_var(pin_iters)

    stage1_key: CVar = CVar(
        "static const uint8_t[]",
        "STAGE1_KEY",
        list(STAGE1_KEY),
    )
    config.add_var(stage1_key)

    stage1_padding: CVar = CVar(
        "static const uint8_t[]",
        "STAGE1_PADDING",
        list(STAGE1_PADDING),
    )
    config.add_var(stage1_padding)

    stage2_rand_int: CVar = CVar(
        "static const uint32_t",
        "STAGE2_RAND_INT",
        STAGE2_RAND_INT,
    )
    config.add_var(stage2_rand_int)

    pin_hash: CVar = CVar(
        "static const uint8_t[]",
        "STAGE1_PIN_HASH",
        list(hash_pin(PIN)),
    )
    config.add_var(pin_hash)

    root_key: CVar = CVar(
        "static const uint8_t[]",
        "ROOT_KEY",
        list(ROOT_KEY),
    )
    config.add_var(root_key)


if __name__ == "__main__":
    main()
