"""
LED Cube Dashboard - Flask Backend
Phase 1: Firmware flashing and Teensy status
"""

import logging
import os
import subprocess
from pathlib import Path

from flask import Flask, jsonify, request
from werkzeug.utils import secure_filename

app = Flask(__name__)

# Configuration
UPLOAD_FOLDER = Path(__file__).parent / "uploads"
ALLOWED_EXTENSIONS = {"hex"}
TEENSY_LOADER_PATH = "/usr/local/bin/teensy_loader_cli"
TEENSY_DEVICE = "/dev/ttyACM0"
TEENSY_MCU = "TEENSY41"

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


@app.route("/api/status", methods=["GET"])
def get_status():
    """Return Teensy connection status."""
    connected = teensy_connected()
    logger.info(f"Status check: Teensy connected = {connected}")
    return jsonify({
        "connected": connected,
        "device": TEENSY_DEVICE if connected else None,
    })


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

    # Flash the Teensy
    try:
        result = subprocess.run(
            [
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
