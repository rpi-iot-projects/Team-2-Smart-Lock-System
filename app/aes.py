from Crypto.Cipher import AES


# 128-bit AES key (same as in STM32)
key = bytes.fromhex('2b7e151628aed2a6abf7158809cf4f3c')

def aes_encrypt(plaintext: bytes) -> bytes:
    cipher = AES.new(key, AES.MODE_ECB)
    if len(plaintext) % 16 != 0:
        raise ValueError("Plaintext must be a multiple of 16 bytes (no padding in ECB mode)")
    return cipher.encrypt(plaintext)

def aes_decrypt(ciphertext: bytes) -> bytes:
    cipher = AES.new(key, AES.MODE_ECB)
    return cipher.decrypt(ciphertext)

