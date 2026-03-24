# RC Car Controller

A PyQt5-based GUI application for controlling an RC car via serial communication with a microcontroller.

## Features

- **Serial Communication**: Connect to microcontrollers via COM/USB ports with configurable baud rates
- **Motor Control**: Control motor direction (Forward/Backward/Disabled) and PWM speed (0-65535)
- **Real-time Feedback**: Live display of sent commands and connection status
- **Debounced Input**: Optimized serial communication with debouncing to prevent UART overload
- **Cross-platform**: Works on Windows and Linux (with Wayland compatibility)

## Requirements

- Python 3.6+
- PyQt5
- pyserial

## Installation

1. **Clone or download the project:**
   ```bash
   git clone <repository-url>
   cd gameOverPy
   ```

2. **Install required dependencies:**
   ```bash
   pip install PyQt5 pyserial
   ```

   Or create a requirements.txt file:
   ```bash
   pip install -r requirements.txt
   ```

## Usage

### Running the Application

```bash
python RC_Car_Control_II_transmitter.py
```

### GUI Controls

1. **Connection Setup:**
   - Select baud rate (9600, 19200, 38400, 57600, 115200)
   - Choose COM port from dropdown
   - Click "Refresh" to update available ports
   - Click "Connect" to establish serial connection

2. **Motor Control:**
   - **Direction Options:**
     - Disabled: Motor is coasted/stopped
     - Forward: Motor runs forward
     - Backward: Motor runs in reverse
   - **PWM Speed Control:**
     - Use the slider to set motor speed (0-65535)
     - Slider is only active when Forward/Backward is selected

3. **Status Monitoring:**
   - Bottom text box displays connection status and sent commands
   - Real-time feedback of all operations

### Serial Communication Protocol

The application sends 32-byte formatted messages to the microcontroller:

- **Format**: `COMMAND,VALUE` padded to 32 bytes with null characters, terminated with newline
- **Commands**:
  - `F,<PWM>` - Forward direction with PWM value
  - `R,<PWM>` - Reverse direction with PWM value  
  - `C,0` - Coast/disable motor

**Example**: `"F,1000"` becomes a 32-byte message: `F,1000` + 25 null bytes + `\n`

## Hardware Requirements

- Microcontroller with UART/serial capability
- Motor driver circuit
- USB-to-serial adapter (if needed)

## Troubleshooting

### Linux/Wayland Issues
If you see the warning: `"qt.qpa.wayland: Wayland does not support QWindow::requestActivate()"`, this is normal and doesn't affect functionality. The application automatically handles this.

To force X11 mode (optional):
```bash
export FORCE_QPA_XCB=1
python RC_Car_Control_II_transmitter.py
```

### Serial Connection Issues
- Ensure the correct COM port is selected
- Verify the microcontroller is connected and powered
- Check that no other applications are using the serial port
- Try different baud rates if communication fails

### Permission Issues (Linux)
Add your user to the dialout group:
```bash
sudo usermod -a -G dialout $USER
```
Then log out and log back in.

## Code Structure

### Main Components

- **MainWindow**: Primary GUI class with all controls and serial communication
- **formatTwoWordData()**: Formats commands into 32-byte protocol messages
- **Debouncing System**: Prevents UART overload during rapid slider movements

### Key Features

- **PWM Debouncing**: Uses QTimer to limit command frequency during slider usage
- **Direction Safety**: Automatically stops motor when switching to "Disabled"
- **Cross-platform Compatibility**: Handles both Windows and Linux environments

## Development

### Adding New Features

The code is structured to easily add new control features:

1. Add GUI elements in `__init__()`
2. Connect signals to handler methods
3. Implement command formatting in handler methods
4. Use `send_serial_data()` to transmit commands

### Protocol Extension

To add new commands:
1. Define new command format (e.g., `"S,<value>"` for servo)
2. Add GUI controls for the new feature
3. Format using `formatTwoWordData()`
4. Send via `send_serial_data()`

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]

## Author

[Add author information here]
