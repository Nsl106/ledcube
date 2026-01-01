# LED Cube Dashboard

Web interface for managing a 16x16x16 LED cube controlled by Teensy 4.1.

## System Architecture

**Hardware:**
- Teensy 4.1 running C++ with OctoWS2812B library
- Raspberry Pi 3B+ running dashboard (WiFi/Ethernet)
- USB connection: Pi → Teensy (flashing + serial communication)

**Software Stack:**
- Backend: Python Flask on Pi
- Frontend: Vite + Svelte + TypeScript + Tailwind
- Teensy flashing: `teensy_loader_cli`
- USB communication: pyserial for animation control

## Repository Structure

Two separate repositories:
1. `ledcube-firmware`: Teensy C++ code, animations, USB protocol docs (PROTOCOL.md)
2. `ledcube-dashboard`: This project - web UI and backend

## Core Features

### Phase 1: Firmware Flashing
- Upload .hex file via web UI
- Flash to Teensy using `teensy_loader_cli`
- Show flash success/failure (no auto-recovery)
- Display Teensy connection status

### Phase 2: Animation Control
- Send animation commands over USB serial (animation ID + parameters)
- Parameters: color (hex), speed (float), other animation-specific settings
- Protocol defined in firmware repo's PROTOCOL.md
- Teensy maintains list of animations, Pi sends high-level commands only

## Key Constraints

**Reliability Requirements:**
- Must be recoverable via WiFi or power cycle only
- Physical button on Teensy accessible for emergency bootloader recovery
- No failure mode should require disassembly

**Network:**
- Primary: WiFi (comitup for fallback AP mode when no network)
- Fallback: Ethernet if WiFi unreliable
- mDNS hostname: `ledcube.local`

## Development Workflow

**Local Development:**
- Frontend: Dev server on PC (`npm run dev`) with API proxy to Pi
- Backend: Runs on Pi (needs USB access to Teensy)
- vite.config.ts proxies `/api/*` to `http://ledcube.local:5000`

**Deployment:**
- Build frontend → static files
- Flask serves built frontend + API
- `deploy.sh` script: build, rsync to Pi, restart service
- Single systemd service on Pi

## Backend (Flask)

**Key Routes:**
- `POST /api/flash` - Upload and flash .hex file
- `GET /api/status` - Teensy connection status, heartbeat
- `POST /api/animation` - Set animation (ID + params: color, speed, etc.)
- Static file serving for built frontend

**Implementation Notes:**
- Subprocess call to `teensy_loader_cli` for flashing
- pyserial for USB serial communication
- Simple error responses (no auto-recovery)

## Frontend (Svelte)

**UI Components:**
- File upload for .hex flashing
- Connection status indicator
- Animation selector with parameter controls (color picker, speed slider)
- Error/success notifications

**Styling:**
- Tailwind CSS
- Minimal comments in code unless complex logic

## USB Protocol

Defined in firmware repo (`ledcube-firmware/PROTOCOL.md`). Dashboard is a client of this protocol. (currently unwritten)

**Expected format (example):**
- Command structure: `{animation_id}:{param1}:{param2}\n`
- Heartbeat: Teensy sends periodic status pings
- Simple ASCII protocol for reliability

## Raspberry Pi Setup

**OS:** Raspberry Pi OS Lite (64-bit)
**Network:** comitup for WiFi fallback/reconfiguration
**Services:** Flask app via systemd, nginx optional

**Dependencies:**
- teensy_loader_cli (ARM64 binary in /usr/local/bin)
- Python 3 + Flask + pyserial
- udev rules for Teensy USB access without sudo

## Future Considerations

- Watchdog: Monitor Teensy heartbeat, log failures
- Known-good firmware backup for manual recovery
- Logging: Flash attempts, serial commands, connection failures