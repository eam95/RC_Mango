# from GarminDialogs import acquireCommandChangeWindow, SensitivityWindow, AcqMeasureSettingsDialog, SemiBurstMeasureSettingsDialog
import sys
from csv import excel

# Ensure Qt environment variables are set before importing PyQt5/initializing Qt.
# On Wayland some Qt calls trigger the warning:
#   "qt.qpa.wayland: Wayland does not support QWindow::requestActivate()"
# We try a non-invasive approach first: silence the Wayland QPA logging category.
# If you prefer to force X11 (via XWayland), set the environment variable
# FORCE_QPA_XCB=1 outside Python (or before running this script) to enable the xcb platform plugin.
import os
if os.environ.get('XDG_SESSION_TYPE', '').lower() == 'wayland' or 'WAYLAND_DISPLAY' in os.environ:
    # Do not override user's existing settings; only set defaults if not present.
    # Silence Wayland QPA warning messages (non-invasive).
    # Note: the exact logging rule may vary across Qt versions; this rule disables the
    # "qt.qpa.wayland" warning category. If you'd rather see other Qt logs, unset
    # QT_LOGGING_RULES in your environment.
    os.environ.setdefault('QT_LOGGING_RULES', 'qt.qpa.wayland.warning=false')
    # Optional fallback: force Qt to use the XCB (X11) platform plugin via XWayland.
    # Enable by exporting FORCE_QPA_XCB=1 in your environment before running the script.
    if os.environ.get('FORCE_QPA_XCB') == '1':
        os.environ.setdefault('QT_QPA_PLATFORM', 'xcb')

from PyQt5.QtWidgets import (QApplication, QMainWindow, QLabel,
                             QComboBox, QPushButton, QTextEdit, QFileDialog,
                             QSlider, QRadioButton, QButtonGroup,
                             QDialog, QVBoxLayout, QHBoxLayout, QLineEdit,
                             QWidget, QMessageBox, QSpinBox)

import serial
import serial.tools.list_ports

from PyQt5.QtCore import Qt
import threading

import time
import os

from PyQt5.QtCore import pyqtSignal, QObject, QTimer




def formatTwoWordData(data):
    # normalize to bytes
    b = data.encode('utf-8') if isinstance(data, str) else bytes(data)
    # produce exactly 32 bytes of content (padded with NUL)
    twoWordFormat = b[:32].ljust(32, b'\0')
    # Add a \n onlast byte
    twoWordFormat = twoWordFormat[:-1] + b'\n'
    return twoWordFormat

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.text_box = QTextEdit(self)
        self.text_box.setGeometry(20, 600, 900, 200)
        self.text_box.setReadOnly(True)


        self.setWindowTitle("RC Car Controller")
        self.setGeometry(100, 100, 1000, 900)

        self.baud_label = QLabel("Select Baud Rate:", self)
        self.baud_label.setGeometry(20, 20, 120, 30)

        self.baud_combo = QComboBox(self)
        self.baud_combo.setGeometry(150, 20, 150, 30)
        self.baud_combo.addItems([
            "9600", "19200", "38400", "57600", "115200"])

        self.com_label = QLabel("Select COM Port:", self)
        self.com_label.setGeometry(20, 70, 120, 30)

        self.com_combo = QComboBox(self)
        self.com_combo.setGeometry(150, 70, 150, 30)
        self.refresh_com_ports()

        self.refresh_btn = QPushButton("Refresh", self)
        self.refresh_btn.setGeometry(320, 70, 120, 30)
        self.refresh_btn.clicked.connect(self.refresh_com_ports)

        self.connect_btn = QPushButton("Connect", self)
        self.connect_btn.setGeometry(100, 130, 120, 40)
        self.connect_btn.clicked.connect(self.connect_to_microcontroller)

        self.disconnect_btn = QPushButton("Disconnect", self)
        self.disconnect_btn.setGeometry(300, 130, 120, 40)
        self.disconnect_btn.clicked.connect(self.disconnect_com_port)

        # Add slider for motor speed control
        self.pwm_label = QLabel("PWM value: 0", self)
        self.pwm_label.setGeometry(20, 190, 120, 30)

        self.pwm_slider = QSlider(Qt.Horizontal, self)
        self.pwm_slider.setGeometry(150, 190, 400, 30)
        self.pwm_slider.setRange(0, 65535)  # use 0-100 if your MCU expects ticks (0-65535)
        self.pwm_slider.setTickInterval(25)
        self.pwm_slider.setEnabled(False)  # enabled when connected
        self.pwm_slider.valueChanged.connect(self.on_pwm_change)

        # Direction radio buttons: Disabled / Forward / Backward
        self.dir_disabled_rb = QRadioButton("Disabled", self)
        self.dir_disabled_rb.setGeometry(570, 180, 100, 30)
        self.dir_forward_rb = QRadioButton("Forward", self)
        self.dir_forward_rb.setGeometry(570, 205, 100, 30)
        self.dir_backward_rb = QRadioButton("Backward", self)
        self.dir_backward_rb.setGeometry(680, 205, 100, 30)

        self.dir_group = QButtonGroup(self)
        self.dir_group.addButton(s elf.dir_disabled_rb)
        self.dir_group.addButton(self.dir_forward_rb)
        self.dir_group.addButton(self.dir_backward_rb)

        # Start with Disabled selected and everything disabled until connected
        self.dir_disabled_rb.setChecked(True)
        self.dir_disabled_rb.setEnabled(False)
        self.dir_forward_rb.setEnabled(False)
        self.dir_backward_rb.setEnabled(False)
        self.current_direction = None  # 'F' or 'B' or None

        # Connect radio button changes
        self.dir_group.buttonClicked.connect(self.on_direction_change)

        # Since there was an issue with UART Receive from responding too quickly to slider changes,
        # debouncing mechanism is added to limit the rate of sending commands.
        self._last_pwm_value = 0
        self.pwm_debounce_timer = QTimer(self)
        self.pwm_debounce_timer.setSingleShot(True)
        self.pwm_debounce_timer.setInterval(10)  # ms; adjust to taste
        self.pwm_debounce_timer.timeout.connect(self.send_debounced_pwm)
        self.pwm_slider.sliderReleased.connect(self.on_slider_released)



    def send_serial_data(self, data):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write(data)
                print(f"Sent '{data}' to {self.serial_port.port} at {self.serial_port.baudrate} baud.")
                data = data.decode(errors='ignore').replace('\0', '').replace('\n', '').strip()
                self.text_box.append(f"Sent: {data}")
            except Exception as e:
                print(f"Error sending data: {e}")
        else:
            print("Serial port is not open.")


    def refresh_com_ports(self):
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.com_combo.clear()
        self.com_combo.addItems(ports if ports else ["No ports found"])
        self.text_box.append(f"COM ports refreshed.")

    def connect_to_microcontroller(self):
        baud = int(self.baud_combo.currentText())
        com = self.com_combo.currentText()
        try:
            self.serial_port = serial.Serial(com, baud, timeout=0.1)
            self.dir_disabled_rb.setEnabled(True)
            self.dir_forward_rb.setEnabled(True)
            self.dir_backward_rb.setEnabled(True)
            self.text_box.append(f"Connected to {com} at {baud} baud.")

        except Exception as e:
            self.text_box.append(f'Error connecting: {e}')

    def disconnect_com_port(self):
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.text_box.append(f"Disconnected from {self.serial_port.port}.")
            self.pwm_slider.setEnabled(False)  # enabled when connected
            self.dir_disabled_rb.setEnabled(False)
            self.dir_forward_rb.setEnabled(False)
            self.dir_backward_rb.setEnabled(False)
        else:
            self.text_box.append("No COM port is currently connected.")

    def on_direction_change(self, button):
        # Determine which radio is selected and enable/disable slider accordingly
        if self.dir_disabled_rb.isChecked():
            self.current_direction = None
            self.pwm_slider.setEnabled(False)
            # send a zero duty to stop the motor
            if hasattr(self, "serial_port") and self.serial_port and self.serial_port.is_open:
                cmd = f"C,0"  # 'C' for coast/disable
                print(cmd)
                print('Coast the motor (disable)')
                cmd = formatTwoWordData(cmd)
                self.send_serial_data(cmd)
            self.text_box.append("Direction: Disabled (slider off)")
            self.pwm_label.setText("PWM: 0")
            self.pwm_slider.setValue(0)
        else:
            # Forward or Backward selected
            if self.dir_forward_rb.isChecked():
                self.current_direction = 'F'
                self.pwm_slider.setValue(0)
                self.text_box.append("Direction: Forward")
            else:
                self.current_direction = 'R'
                self.pwm_slider.setValue(0)
                self.text_box.append("Direction: Backward")
            # enable slider so user can set duty
            self.pwm_slider.setEnabled(True)
            self.on_pwm_change(0)  # send initial 0% duty

    def on_pwm_change(self, value):
        # update UI immediately, but defer sending
        self._last_pwm_value = value
        self.pwm_label.setText(f"PWM: {value}")
        self.text_box.append(f"Setting pwm to {value}")
        # restart debounce timer so send happens after user stops moving slider
        self.pwm_debounce_timer.start()

    # Add these helper methods to MainWindow
    def on_slider_released(self):
        # send immediately on release (final value)
        if self.pwm_debounce_timer.isActive():
            self.pwm_debounce_timer.stop()
        self.send_debounced_pwm()

    def send_debounced_pwm(self):
        value = getattr(self, "_last_pwm_value", self.pwm_slider.value())
        if hasattr(self, "serial_port") and self.serial_port and self.serial_port.is_open and self.current_direction:
            cmd = f"{self.current_direction},{value}"
            cmd = formatTwoWordData(cmd)
            self.send_serial_data(cmd)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())