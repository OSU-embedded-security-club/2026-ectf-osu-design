"""
Author: Andrew Langan
Date: 2025

This file contains helpers to generate cryptographic keys

Copyright: Copyright (c) 2025
"""

from ctypes import Array, c_uint8

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher
from cryptography.hazmat.primitives.ciphers.algorithms import AES
from cryptography.hazmat.primitives.ciphers.base import CipherContext
from cryptography.hazmat.primitives.ciphers.modes import CTR
from cryptography.hazmat.primitives.hashes import SHA256, Hash
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives.serialization import (
    Encoding,
    NoEncryption,
    PublicFormat,
    PrivateFormat,
)
from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature


def endian_swapB(
    data: bytes,
) -> bytes:
    """
    Swap the endianness of a byte array

    Args:
        data (bytes): Data to swap

    Returns:
        bytes: Swapped data
    """

    b: bytes = b""
    assert len(data) % 4 == 0, "Data must be a multiple of 4 bytes"

    for i in range(len(data) // 4):
        b += endian_swap32(int.from_bytes(data[i * 4 : (i + 1) * 4], "big")).to_bytes(
            4, "big"
        )

    return b


def endian_swapA(
    data: Array[c_uint8],
) -> Array[c_uint8]:
    """
    Swap the endianness of a byte array

    Args:
        data (Array[c_uint8]): Data to swap

    Returns:
        Array[c_uint8]: Swapped data
    """

    return (c_uint8 * len(data))(*endian_swapB(bytes(data)))


def endian_swap64(
    data: int,
) -> int:
    """
    Swap the endianness of a 64 bit integer

    Args:
        data (int): Data to swap

    Returns:
        int: Swapped data
    """

    _data: bytes = data.to_bytes(8, "big")
    sw: int = int.from_bytes(_data[:4], "little") << 32
    sw |= int.from_bytes(_data[4:], "little")
    return sw


def endian_swap32(
    data: int,
) -> int:
    """
    Swap the endianness of a 32 bit integer

    Args:
        data (int): Data to swap

    Returns:
        int: Swapped data
    """

    return int.from_bytes(data.to_bytes(4, "big"), "little")


def derive_params(
    priv: "KeyPair",
    pub: "KeyPair",
    ctr: int,
) -> tuple[bytes, bytes]:
    """
    Derive a nonce and key from two keypairs

    Args:
        priv (KeyPair): Private keypair
        pub (KeyPair): Public keypair

    Returns:
        tuple[bytes,bytes]: Key and nonce
    """

    hasher: Hash = Hash(SHA256(), default_backend())
    shared_secret: bytes = priv.xkey.exchange(pub.xkey.public_key())
    hasher.update(shared_secret)

    hash: bytes = hasher.finalize()

    return hash[:24], hash[24:] + b"\0x\0SU" + ctr.to_bytes(4, "big")


def derive_keypair(
    priv: "KeyPair",
    pub: "KeyPair",
    ctr: int,
) -> CipherContext:
    """
    Derive an AES CipherContext from two keypairs

    Args:
        priv (KeyPair): Private keypair
        pub (KeyPair): Public keypair
        ctr (int): Counter for the nonce

    Returns:
        CipherContext: AES CipherContext
    """

    key, nonce = derive_params(priv, pub, ctr)
    return Cipher(AES(key), CTR(nonce), default_backend()).encryptor()


class KeyPair:
    """
    Dataclass to hold a keypair
    """

    key: Ed25519PrivateKey
    xkey: X25519PrivateKey
    pub: bytes
    priv: bytes

    def __init__(
        self,
        seed: str,
        salt: str,
        id: int,
    ) -> None:
        _seed: int = int.from_bytes(bytes.fromhex(seed), "big")
        full_seed: bytes = _seed.to_bytes(64, "big") + id.to_bytes(4, "big")

        key: bytes = PBKDF2HMAC(
            SHA256(), 32, salt.encode(), 10_000_000, default_backend()
        ).derive(full_seed)

        self.key = Ed25519PrivateKey.from_private_bytes(key)
        self.xkey = X25519PrivateKey.from_private_bytes(key)

        assert (
            self.key.public_key().public_bytes(
                Encoding.X962, PublicFormat.UncompressedPoint
            )[1:]
            == self.xkey.public_key().public_bytes(
                Encoding.X962, PublicFormat.UncompressedPoint
            )[1:]
        ), "Public keys do not match"

        self.priv = self.key.private_bytes(
            Encoding.X962,
            PrivateFormat.Raw,
            NoEncryption(),
        )
        self.pub = self.key.public_key().public_bytes(
            Encoding.X962, PublicFormat.UncompressedPoint
        )[1:]

    def priv_array(
        self,
    ) -> Array[c_uint8]:
        """Convert the private key to a byte array

        Returns:
            Array[c_uint8]: Private key as a byte array
        """

        return (c_uint8 * 32)(*self.priv)

    def pub_array(
        self,
    ) -> Array[c_uint8]:
        """Convert the public key to a byte array

        Returns:
            Array[c_uint8]: Public key as a byte array
        """

        return (c_uint8 * 64)(*self.pub)

    def sign(
        self,
        data: bytes,
    ) -> bytes:
        """
        Sign data with the private key

        Args:
            data (bytes): Data to sign

        Returns:
            bytes: Signature
        """

        hasher: Hash = Hash(SHA256(), default_backend())
        hasher.update(data)
        hash: bytes = hasher.finalize()

        sig: bytes = self.key.sign(hash)
        r, s = decode_dss_signature(sig)

        return r.to_bytes(32, "big") + s.to_bytes(32, "big")

    def sign_array(
        self,
        data: bytes,
    ) -> Array[c_uint8]:
        """
        Sign data with the private key and convert to a byte array

        Args:
            data (bytes): Data to sign

        Returns:
            Array[c_uint8]: Signature as a byte array
        """

        return (c_uint8 * 64)(*self.sign(data))

    def encrypt(
        self,
        data: bytes,
        other: "KeyPair",
        ctr: int = 0,
    ) -> bytes:
        """
        Encrypt data with a shared secret between two keys

        Args:
            data (bytes): Data to encrypt

        Returns:
            bytes: Encrypted data
        """

        cipher: CipherContext = derive_keypair(self, other, ctr)

        return cipher.update(data) + cipher.finalize()


class AESState:
    """
    Class to hold the state of an AES encryption
    """

    key: bytes
    nonce: bytes
    ctr: int

    def __init__(
        self,
        priv: KeyPair,
        pub: KeyPair,
    ) -> None:
        self.key, self.nonce = derive_params(priv, pub, 0)
        self.ctr = 0

    def encrypt(self, data: bytes) -> bytes:
        """
        Encrypt data with the AES state

        Args:
            data (bytes): Data to encrypt

        Returns:
            bytes: Encrypted data
        """

        assert len(data) % 16 == 0, "Data must be a multiple of 16 bytes"

        cipher = Cipher(
            AES(self.key),
            CTR(self.nonce),
            default_backend(),
        )

        encryptor: CipherContext = cipher.encryptor()
        ciphertext = encryptor.update(data) + encryptor.finalize()

        self.ctr += len(data) // 16
        self.nonce = self.nonce[:12] + self.ctr.to_bytes(4, "big")

        return ciphertext
