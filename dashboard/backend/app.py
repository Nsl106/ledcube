"""
LED Cube Dashboard - Flask Backend
Phase 1: Firmware flashing and Teensy status
Phase 2: Animation control via serial
"""

import atexit
import logging
import subprocess
import threading
from pathlib import Path

import serial
from flask import Flask, jsonify, request
from werkzeug.utils import secure_filename

app = Flask(__name__)

# Configuration
UPLOAD_FOLDER = Path(__file__).parent / "uploads"
ALLOWED_EXTENSIONS = {"hex"}
TEENSY_LOADER_PATH = "/usr/local/bin/teensy_loader_cli"
TEENSY_DEVICE = "/dev/ttyACM0"
TEENSY_MCU = "TEENSY41"

# Serial configuration
SERIAL_BAUD = 115200
SERIAL_TIMEOUT = 1.0

# Animation definitions - must match firmware
# Parameter types: "float", "int", "color"
ANIMATIONS = {
    0: {
        "id": 0,
        "name": "Twinkle Fade",
        "description": "Fading twinkling stars",
        "params": [
            {"key": "fadeRate", "label": "Fade Rate", "type": "float", "min": 0.9, "max": 0.999, "default": 0.98, "step": 0.001},
            {"key": "spawnChance", "label": "Spawn Rate", "type": "float", "min": 0.0001, "max": 0.01, "default": 0.001, "step": 0.0001},
            {"key": "color", "label": "Color", "type": "color", "default": "FF6347"},
        ],
    },
    1: {
        "id": 1,
        "name": "RGB Color Shift",
        "description": "Rainbow wave effect",
        "params": [
            {"key": "brightness", "label": "Brightness", "type": "float", "min": 0.1, "max": 1.0, "default": 0.5, "step": 0.1},
            {"key": "size", "label": "Wave Size", "type": "float", "min": 1, "max": 50, "default": 16, "step": 1},
            {"key": "speed", "label": "Speed", "type": "float", "min": 0.5, "max": 20, "default": 4, "step": 0.5},
        ],
    },
    2: {
        "id": 2,
        "name": "Twinkle",
        "description": "Static twinkling lights",
        "params": [
            {"key": "num", "label": "Count", "type": "int", "min": 10, "max": 500, "default": 50, "step": 10},
            {"key": "color", "label": "Color", "type": "color", "default": "FFFFFF"},
            {"key": "bg", "label": "Background", "type": "color", "default": "000000"},
            {"key": "delay", "label": "Delay (ms)", "type": "int", "min": 10, "max": 500, "default": 100, "step": 10},
        ],
    },
    3: {
        "id": 3,
        "name": "Random",
        "description": "Random color noise",
        "params": [
            {"key": "brightness", "label": "Brightness", "type": "float", "min": 0.1, "max": 1.0, "default": 0.5, "step": 0.1},
            {"key": "duration", "label": "Duration (ms)", "type": "float", "min": 10, "max": 500, "default": 100, "step": 10},
        ],
    },
    4: {
        "id": 4,
        "name": "Solid",
        "description": "Single solid color",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "0000FF"},
        ],
    },
    5: {
        "id": 5,
        "name": "Fire",
        "description": "Fire simulation",
        "params": [],
    },
    6: {
        "id": 6,
        "name": "Fill",
        "description": "Layer-by-layer fill",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "0000FF"},
            {"key": "axis", "label": "Axis", "type": "select", "options": [
                {"value": "y", "label": "Y (Bottom→Top)"},
                {"value": "z", "label": "Z (Front→Back)"},
                {"value": "x", "label": "X (Left→Right)"},
            ], "default": "y"},
            {"key": "direction", "label": "Direction", "type": "select", "options": [
                {"value": "+", "label": "Forward"},
                {"value": "-", "label": "Reverse"},
            ], "default": "+"},
            {"key": "speed", "label": "Speed", "type": "float", "min": 0.5, "max": 5.0, "default": 1.0, "step": 0.5},
        ],
    },
    7: {
        "id": 7,
        "name": "Plane Sweep",
        "description": "Diagonal plane sweep",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "00FFFF"},
            {"key": "plane", "label": "Plane", "type": "select", "options": [
                {"value": "xyz", "label": "XYZ (Corner)"},
                {"value": "xy", "label": "XY Diagonal"},
                {"value": "xz", "label": "XZ Diagonal"},
                {"value": "yz", "label": "YZ Diagonal"},
            ], "default": "xyz"},
            {"key": "speed", "label": "Speed", "type": "float", "min": 0.5, "max": 5.0, "default": 1.0, "step": 0.5},
            {"key": "thickness", "label": "Thickness", "type": "float", "min": 1.0, "max": 8.0, "default": 2.0, "step": 0.5},
        ],
    },
    8: {
        "id": 8,
        "name": "Rain",
        "description": "Falling raindrops with trails",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "0064FF"},
            {"key": "spawnRate", "label": "Density", "type": "float", "min": 0.05, "max": 0.5, "default": 0.15, "step": 0.05},
            {"key": "speed", "label": "Speed", "type": "float", "min": 0.1, "max": 1.0, "default": 0.3, "step": 0.1},
            {"key": "trailLength", "label": "Trail Length", "type": "float", "min": 1.0, "max": 8.0, "default": 4.0, "step": 0.5},
        ],
    },
    9: {
        "id": 9,
        "name": "Test",
        "description": "Test",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "0064FF"},
            {"key": "x", "label": "X", "type": "int", "min": 0, "max": 15, "default": 0, "step": 1},
            {"key": "y", "label": "Y", "type": "int", "min": 0, "max": 15, "default": 0, "step": 1},
            {"key": "z", "label": "Z", "type": "int", "min": 0, "max": 15, "default": 0, "step": 1},
        ],
    },
    10: {
        "id": 10,
        "name": "CenterPulse",
        "description": "CenterPulse",
        "params": [
            {"key": "color", "label": "Color", "type": "color", "default": "2D002D"},
            {"key": "speed", "label": "Speed", "type": "float", "min": 0, "max": 1, "default": 0.5, "step": 0.01},
            {"key": "style", "label": "Style", "type": "select", "options": [
                {"value": "fill", "label": "Fill"},
                {"value": "edges", "label": "Edges"},
            ], "default": "fill"},
        ],
    }
}

app.config["UPLOAD_FOLDER"] = UPLOAD_FOLDER
app.config["MAX_CONTENT_LENGTH"] = 2 * 1024 * 1024  # 2MB max

# Logging setup
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s",
)
logger = logging.getLogger(__name__)


def allowed_file(filename: str) -> bool:
    return "." in filename and filename.rsplit(".", 1)[1].lower() in ALLOWED_EXTENSIONS


def teensy_connected() -> bool:
    """Check if Teensy is connected via USB."""
    return Path(TEENSY_DEVICE).exists()


class SerialManager:
    """Manages serial connection with locking to prevent concurrent access."""

    def __init__(self):
        self._lock = threading.Lock()
        self._connection: serial.Serial | None = None

    def _ensure_connected(self) -> serial.Serial | None:
        """Ensure we have an open connection, reconnecting if needed."""
        if not teensy_connected():
            self._close()
            return None

        if self._connection is None or not self._connection.is_open:
            try:
                self._connection = serial.Serial(
                    TEENSY_DEVICE, SERIAL_BAUD, timeout=SERIAL_TIMEOUT
                )
                logger.info("Serial connection opened")
            except serial.SerialException as e:
                logger.error(f"Failed to open serial: {e}")
                self._connection = None
                return None

        return self._connection

    def _close(self):
        """Close the serial connection."""
        if self._connection and self._connection.is_open:
            try:
                self._connection.close()
                logger.info("Serial connection closed")
            except Exception:
                pass
        self._connection = None

    def send_command(self, command: str) -> tuple[bool, str]:
        """Send command to Teensy with locking."""
        with self._lock:
            conn = self._ensure_connected()
            if conn is None:
                return False, "Teensy not connected"

            try:
                # Clear any pending input
                conn.reset_input_buffer()

                conn.write(f"{command}\n".encode())
                response = conn.readline().decode().strip()

                if response == "OK":
                    return True, "Command accepted"
                elif response.startswith("ERR:"):
                    return False, response[4:]
                else:
                    return True, "Command sent"
            except serial.SerialException as e:
                logger.error(f"Serial error: {e}")
                self._close()  # Force reconnect on next attempt
                return False, f"Serial error: {str(e)}"

    def close_for_flash(self):
        """Close connection before flashing firmware."""
        with self._lock:
            self._close()

    def cleanup(self):
        """Clean up on shutdown."""
        self._close()


# Global serial manager instance
serial_manager = SerialManager()
atexit.register(serial_manager.cleanup)


@app.route("/api/status", methods=["GET"])
def get_status():
    """Return Teensy connection status."""
    connected = teensy_connected()
    logger.info(f"Status check: Teensy connected = {connected}")
    return jsonify({
        "connected": connected,
        "device": TEENSY_DEVICE if connected else None,
    })


@app.route("/api/animations", methods=["GET"])
def get_animations():
    """Return list of available animations."""
    return jsonify({"animations": list(ANIMATIONS.values())})


@app.route("/api/animation", methods=["POST"])
def set_animation():
    """Set the current animation by ID with optional parameters."""
    data = request.get_json()

    if not data or "id" not in data:
        return jsonify({"success": False, "error": "Missing animation ID"}), 400

    animation_id = data["id"]

    if animation_id not in ANIMATIONS:
        return jsonify({"success": False, "error": "Invalid animation ID"}), 400

    anim = ANIMATIONS[animation_id]
    params = data.get("params", {})

    # Build command with parameters in order
    cmd_parts = [str(animation_id)]
    for param_def in anim["params"]:
        key = param_def["key"]
        value = params.get(key, param_def["default"])
        # Colors are passed as hex strings without #
        if param_def["type"] == "color":
            value = str(value).lstrip("#").upper()
        cmd_parts.append(str(value))

    command = f"ANIM:{':'.join(cmd_parts)}"
    success, message = serial_manager.send_command(command)

    if success:
        logger.info(f"Animation set to: {anim['name']} with params {params}")
        return jsonify({
            "success": True,
            "animation": anim,
            "message": message,
        })
    else:
        logger.error(f"Failed to set animation: {message}")
        return jsonify({"success": False, "error": message}), 500


@app.route("/api/flash", methods=["POST"])
def flash_firmware():
    """Upload and flash .hex file to Teensy."""
    # Check if file was uploaded
    if "file" not in request.files:
        logger.warning("Flash request missing file")
        return jsonify({"success": False, "error": "No file provided"}), 400

    file = request.files["file"]

    if file.filename == "":
        logger.warning("Flash request with empty filename")
        return jsonify({"success": False, "error": "No file selected"}), 400

    if not allowed_file(file.filename):
        logger.warning(f"Invalid file type: {file.filename}")
        return jsonify({"success": False, "error": "Only .hex files allowed"}), 400

    # Ensure upload directory exists
    UPLOAD_FOLDER.mkdir(parents=True, exist_ok=True)

    # Save file
    filename = secure_filename(file.filename)
    filepath = UPLOAD_FOLDER / filename

    try:
        file.save(filepath)
        logger.info(f"Saved firmware file: {filepath}")
    except Exception as e:
        logger.error(f"Failed to save file: {e}")
        return jsonify({"success": False, "error": "Failed to save file"}), 500

    # Close serial connection before flashing
    serial_manager.close_for_flash()

    # Flash the Teensy (use sudo for USB access - configured via sudoers)
    try:
        result = subprocess.run(
            [
                "sudo",
                TEENSY_LOADER_PATH,
                "--mcu", TEENSY_MCU,
                "-w",  # Wait for device
                "-s",  # Soft reboot (trigger bootloader via USB)
                "-v",  # Verbose
                str(filepath),
            ],
            capture_output=True,
            text=True,
            timeout=60,
        )

        if result.returncode == 0:
            logger.info(f"Flash successful: {filename}")
            return jsonify({
                "success": True,
                "message": "Firmware flashed successfully",
                "output": result.stdout,
            })
        else:
            logger.error(f"Flash failed: {result.stderr}")
            return jsonify({
                "success": False,
                "error": "Flash failed",
                "details": result.stderr or result.stdout,
            }), 500

    except subprocess.TimeoutExpired:
        logger.error("Flash timeout - Teensy may not be in bootloader mode")
        return jsonify({
            "success": False,
            "error": "Flash timeout - ensure Teensy is in bootloader mode",
        }), 500
    except FileNotFoundError:
        logger.error(f"teensy_loader_cli not found at {TEENSY_LOADER_PATH}")
        return jsonify({
            "success": False,
            "error": "Flashing tool not installed",
        }), 500
    except Exception as e:
        logger.error(f"Flash error: {e}")
        return jsonify({"success": False, "error": str(e)}), 500
    finally:
        # Clean up uploaded file
        try:
            filepath.unlink()
            logger.info(f"Cleaned up: {filepath}")
        except Exception:
            pass


if __name__ == "__main__":
    logger.info("Starting LED Cube Dashboard backend")
    app.run(host="0.0.0.0", port=5000, debug=True)
