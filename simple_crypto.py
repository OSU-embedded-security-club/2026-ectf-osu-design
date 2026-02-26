"""
Author: Andrew Langan
Date: 2026

This file contains helpers to generate cryptographic keys

Copyright: Copyright (c) 2025
"""

import hashlib
import os
from ctypes import Array, c_uint8

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.hazmat.primitives.hashes import BLAKE2b, Hash
from cryptography.hazmat.primitives.serialization import (
    Encoding,
    NoEncryption,
    PrivateFormat,
    PublicFormat,
)


def endian_swap_bytes(
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


def endian_swap_array(
    data: Array[c_uint8],
) -> Array[c_uint8]:
    """
    Swap the endianness of a byte array

    Args:
        data (Array[c_uint8]): Data to swap

    Returns:
        Array[c_uint8]: Swapped data
    """

    return (c_uint8 * len(data))(*endian_swap_bytes(bytes(data)))


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
) -> tuple[bytes, bytes]:
    """
    Derive a nonce and key from two keypairs

    Args:
        priv (KeyPair): Private keypair
        pub (KeyPair): Public keypair

    Returns:
        tuple[bytes,bytes]: Key and nonce
    """

    hasher: Hash = Hash(BLAKE2b(64), default_backend())
    shared_secret: bytes = priv.xkey.exchange(pub.xkey.public_key())
    hasher.update(shared_secret)

    hash: bytes = hasher.finalize()

    key: bytes = hash[:16]
    nonce: bytes = hash[24:] + b"\0x\0SU"

    assert len(key) == 16, "Key must be 16 bytes long"
    assert len(nonce) == 12, "Nonce must be 12 bytes long"

    return key, nonce


def derive_keypair(
    priv: "KeyPair",
    pub: "KeyPair",
) -> tuple[AESGCM, bytes]:
    """
    Derive an AES CipherContext from two keypairs

    Args:
        priv (KeyPair): Private keypair
        pub (KeyPair): Public keypair

    Returns:
        tuple[AESGCM, bytes]: AES GCM context and nonce
    """

    key, nonce = derive_params(priv, pub)
    return AESGCM(key), nonce


class RNG:
    """
    Class to hold a random number generator
    """

    @staticmethod
    def rand(
        num_bytes: int,
    ) -> bytes:
        """
        Generate a random byte string of a given length

        Args:
            num_bytes (int): Number of bytes to generate

        Returns:
            bytes: Random byte string
        """

        return os.urandom(num_bytes)


class Hasher:
    """
    Class to hold a hasher
    """

    @staticmethod
    def hash(
        rounds: int,
        key: bytes,
        data: bytes,
    ) -> bytes:
        """
        Run a hash function for a number of rounds

        Args:
            rounds (int): Number of rounds to run
            key (bytes): Key to use for the hash
            data (bytes): Data to update with
        """

        hash: bytes = hashlib.blake2b(data, key=key).digest()

        for _ in range(rounds - 1):
            hash = hashlib.blake2b(hash, key=key).digest()

        return hash


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
        key: bytes = os.urandom(32),
    ) -> None:
        """
        Initialize the keypair with a random or provided key

        Args:
            key (bytes, optional): The key to use for the keypair. Defaults to os.urandom(32).
        """

        assert len(key) == 32, "Key must be 32 bytes long"

        self.key = Ed25519PrivateKey.from_private_bytes(key)
        self.xkey = X25519PrivateKey.from_private_bytes(key)

        assert (
            self.key.public_key().public_bytes(
                Encoding.X962,
                PublicFormat.UncompressedPoint,
            )[1:]
            == self.xkey.public_key().public_bytes(
                Encoding.X962,
                PublicFormat.UncompressedPoint,
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

        hasher: Hash = Hash(BLAKE2b(64), default_backend())
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
    ) -> tuple[bytes, bytes]:
        """
        Encrypt data with a shared secret between two keys

        Args:
            data (bytes): Data to encrypt
            other (KeyPair): Other keypair to derive the shared secret with

        Returns:
            tuple[bytes, bytes]: Encrypted data and tag
        """

        cipher, nonce = derive_keypair(self, other)

        enc: bytes = cipher.encrypt(nonce, data, None)

        return enc[:-16], enc[-16:]


class AESState:
    """
    Class to hold the state of an AES encryption
    """

    key: bytes
    nonce: bytes

    def __init__(
        self,
        priv: KeyPair,
        pub: KeyPair,
    ) -> None:
        self.key, self.nonce = derive_params(priv, pub)

    def encrypt(
        self,
        data: bytes,
    ) -> tuple[bytes, bytes]:
        """
        Encrypt data with the AES state

        Args:
            data (bytes): Data to encrypt

        Returns:
            tuple[bytes, bytes]: Encrypted data and tag
        """

        assert len(data) % 16 == 0, "Data must be a multiple of 16 bytes"

        cipher: AESGCM = AESGCM(self.key)

        _nonce: bytes = self.nonce
        ciphertext: bytes = cipher.encrypt(self.nonce, data, None)
        assert self.nonce != _nonce, "Nonce was not updated!"

        return ciphertext[:-16], ciphertext[-16:]

    def decrypt(
        self,
        data: bytes,
    ) -> bytes:
        """
        Decrypt data with the AES state

        Args:
            data (bytes): Data to decrypt

        Returns:
            bytes: Decrypted data
        """

        assert len(data) % 16 == 0, "Data must be a multiple of 16 bytes"

        cipher: AESGCM = AESGCM(self.key)

        _nonce: bytes = self.nonce
        plaintext: bytes = cipher.decrypt(self.nonce, data, None)
        assert self.nonce != _nonce, "Nonce was not updated!"

        return plaintext
