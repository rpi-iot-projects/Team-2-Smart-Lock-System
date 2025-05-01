import os
import re
import sys
import time
import cv2


from PyQt5 import Qt
from PyQt5.QtCore import QTimer, QThread
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QTextEdit, QPlainTextEdit
)
from PyQt5.QtGui import QPixmap, QFont, QPainter, QPen, QPaintEvent, QImage
from lock_socket import *
from settings import *

button_text = [
    "Connect",
    "Unlock",
    "Setup OTP",
    "Disable OTP",
    "Open camera",
    "Close camera",
    "New Password",
    "New Fingerprint"
]


class CommandButton(QPushButton):
    def __init__(self, callback, button_id, text, parent=None):
        super().__init__(text, parent)
        self.button_id = button_id
        self.clicked.connect(self.on_click)
        self.callback = callback

    def on_click(self):
        self.callback(self.button_id)


class RollingTextRegion(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.text_edit = QTextEdit(parent)
        self.text_edit.setReadOnly(True)
        self.text_edit.setPlaceholderText("Password")

    def add_line(self, text):
        self.text_edit.append(text)
        self.text_edit.verticalScrollBar().setValue(
            self.text_edit.verticalScrollBar().maximum()
        )
        QApplication.processEvents()


class MyApp(QWidget):
    def __init__(self):
        super().__init__()
        self.current_index = None
        self.cap = None
        self.temp_pwd = None
        self.pwd = None
        self.text_region = None
        self.buttons = None
        self.image_label = None
        self.key_event_enabled = True
        self.init_ui()

        # -- Camera -------------
        self.frame_dir = IMAGE_FOLDER
        self.placeholder_path = "./off_image.png"
        self.fps = 30
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.next_frame)
        # -----------------

        # self.socket = TCPClient(self, DEVICE_IP, DEVICE_PORT)
        self.socket = TCPClient(self)

        self.thread = WorkerThread(self.socket)

    def init_ui(self):
        self.setWindowTitle('Platform')
        self.setGeometry(100, 100, 600, 500)  # Main window size

        # Image
        self.image_label = QLabel(self)
        self.image_label.setGeometry(20, 20, 240, 220)
        self.update_frame('./off_image.png')

        # Report
        self.text_region = RollingTextRegion(self)
        self.text_region.text_edit.setGeometry(280, 20, 270, 200)

        # password Rectangle
        self.pwd = QTextEdit(self)
        self.pwd.setGeometry(280, 230, 100, 30)
        self.pwd.setPlaceholderText("Password")

        self.temp_pwd = QTextEdit(self)
        self.temp_pwd.setGeometry(420, 230, 100, 30)
        self.temp_pwd.setPlaceholderText("Temp Pwd")

        # Buttons
        self.buttons = []
        start_x = 50
        start_y = 300
        button_width = 100
        button_height = 40
        gap = 30

        for i in range(8):
            btn = CommandButton(self.handle_cmd, i + 1, button_text[i], self)
            if i < 4:
                btn.setGeometry(start_x + i * (button_width + gap), start_y, button_width, button_height)
            else:
                btn.setGeometry(start_x + (i-4) * (button_width + gap), start_y + 60, button_width, button_height)
            self.buttons.append(btn)

        self.show()
        self.text_region.add_line("Welcome")

    def update_frame(self, path):
        pixmap = QPixmap(path)
        if not pixmap.isNull():
            scaled = pixmap.scaled(self.image_label.width(), self.image_label.height())
            self.image_label.setPixmap(scaled)

    def handle_cmd(self, button_id):
        match button_id:
            case 1:
                self.socket.connect()
                if self.socket.keep_running:
                    pass
                    self.thread.start()
            case 2:
                self.socket.transmit(1)
            case 3:
                self.text_region.add_line("Temp Password Set")
                self.socket.transmit(6, self.temp_pwd.toPlainText())
            case 4:
                self.text_region.add_line("Temp Password Removed")
                self.socket.transmit(7)
            case 5:
                self.text_region.add_line("Start Camera")
                self.socket.transmit(3)
                time.sleep(2)
                self.start_play()
            case 6:
                self.text_region.add_line("Close Camera")
                self.socket.transmit(4)
                self.stop_play()
            case 7:
                self.text_region.add_line("New Password Set")
                self.socket.transmit(6, self.pwd.toPlainText())
            case 8:
                self.socket.transmit(8)

    def start_play(self):
        self.timer.start(1000 // self.fps)

    def stop_play(self):
        self.timer.stop()
        self.update_frame(self.placeholder_path)

    def next_frame(self):
        path = os.path.join(self.frame_dir, self.frame_files[self.current_index])
        self.update_frame(path)

    def get_recent_image_path(self):
        max_k = -1
        max_k_filename = None
        pattern = re.compile(r'5MP_MC_(\d+)_\d+\.png')  # Regex to capture the 'k' value
        for filename in os.listdir(self.frame_dir):
            match = pattern.match(filename)
            if match:
                k_value = int(match.group(1))
                if k_value > max_k:
                    max_k = k_value
                    max_k_filename = filename
        if max_k_filename:
            path = os.path.join(self.frame_dir, max_k_filename)
            return path
        else:
            return None


if __name__ == '__main__':
    app = QApplication(sys.argv)
    platform = MyApp()
    sys.exit(app.exec_())
