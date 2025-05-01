import socket

from PyQt5.QtCore import QThread

from aes import *
from settings import *
import random


def construct_bytes(a, b):
    result = bytearray()

    # Bytes 1-2: 1
    result += (1).to_bytes(2, byteorder='big')

    # Byte 3: a
    result.append(a)

    # Bytes 4+: from content
    for digit in b:
        result.append(0x10 + int(digit))

    result.append(0x20)  # Add terminate byte

    while len(result) < 16:  # Pad with 0x00
        result.append(0x00)
    return bytes(result[:16])


class TCPClient:
    def __init__(self, app, host='172.16.13.200', port=24928):
        self.cnt = 0
        self.keep_running = False
        self.report = None
        self.host = host
        self.port = port
        self.sock = None
        self.app = app

    def connect(self):
        if self.app:
            self.report = self.app.text_region
        """Establish a TCP connection to the server."""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        self.sock.settimeout(1.0)
        if self.report:
            self.report.add_line(f"Connects to {self.host}/{self.port}")
            self.keep_running = True

    def run(self):
        while self.keep_running:
            try:
                rxdata = self.sock.recv(16)
                if rxdata:
                    lock_report = aes_decrypt(rxdata)
                    self.decode(lock_report)

            except socket.timeout:
                continue  # No data, loop again
            except Exception as e:
                print("Socket error:", e)
                break

    def send_16_bytes(self, data: bytes):
        """Send up to 16 bytes. Pad with null bytes if shorter."""
        if not isinstance(data, bytes):
            raise TypeError("Data must be bytes")
        if len(data) > 16:
            raise ValueError("Data must be 16 bytes or less")

        padded = data.ljust(16, b'\x00')  # pad with null bytes if needed
        encoded = aes_encrypt(padded)
        self.sock.sendall(encoded)

    def close(self):
        if self.sock:
            self.sock.close()
            print("[CLIENT] Connection closed.")
            self.sock = None

    def transmit(self, cmd, content=None):
        if content is None:
            packet = construct_bytes(cmd, ''.join(random.choices('0123456789', k=12)))
        else:
            packet = construct_bytes(cmd, content)
        self.send_16_bytes(packet)

    def handle_report(self, report):
        if report[2] == 2:
            match report[3]:
                case 1:
                    self.report.add_line(f"[L] Unlocked using Password")
                case 2:
                    self.report.add_line(f"[L] Unlocked using Temp password")
                case 3:
                    self.report.add_line(f"[L] Unlocked using Fingerprint")
        elif report[2] == 2:
            self.report.add_line(f"[L] Locked")
        elif report[2] == 3:
            match report[3]:
                case 1:
                    self.report.add_line(f"[L] Denied access using Password")
    

class WorkerThread(QThread):
    def __init__(self, client):
        super().__init__()
        self.client = client

    def run(self):
        self.client.run()



