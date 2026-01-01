# LED Cube Dashboard - Initial Setup Script
# Run once to set up the Pi

param(
    [string]$PiHost = "ledcube.local",
    [string]$User = "ledcube",
    [string]$RemotePath = "/home/ledcube/ledcube-dashboard"
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "=== LED Cube Dashboard Setup ===" -ForegroundColor Cyan
Write-Host "Target: ${User}@${PiHost}:${RemotePath}"
Write-Host ""

# Test connection
Write-Host "Testing connection..." -ForegroundColor Yellow
ssh "${User}@${PiHost}" "echo 'Connected successfully'"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Failed to connect to Pi. Check that:" -ForegroundColor Red
    Write-Host "  - Pi is powered on and connected to network"
    Write-Host "  - SSH is enabled on the Pi"
    Write-Host "  - You can resolve ${PiHost} (try ping ${PiHost})"
    exit 1
}

# Install teensy_loader_cli
Write-Host "Checking for teensy_loader_cli..." -ForegroundColor Yellow
$hasTeensy = ssh "${User}@${PiHost}" "which teensy_loader_cli 2>/dev/null"

if (-not $hasTeensy) {
    Write-Host "Installing teensy_loader_cli build dependencies..." -ForegroundColor Yellow
    ssh "${User}@${PiHost}" "sudo apt-get update && sudo apt-get install -y build-essential libusb-dev"

    Write-Host "Building teensy_loader_cli from source..." -ForegroundColor Yellow
    ssh "${User}@${PiHost}" @"
cd /tmp &&
rm -rf teensy_loader_cli &&
git clone https://github.com/PaulStoffregen/teensy_loader_cli.git &&
cd teensy_loader_cli &&
make &&
sudo cp teensy_loader_cli /usr/local/bin/ &&
rm -rf /tmp/teensy_loader_cli
"@

    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: Failed to build teensy_loader_cli" -ForegroundColor Red
        exit 1
    }

    # Set up udev rules for Teensy USB access without sudo
    Write-Host "Setting up udev rules for Teensy..." -ForegroundColor Yellow
    ssh "${User}@${PiHost}" @"
echo 'ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE="0666"' | sudo tee /etc/udev/rules.d/49-teensy.rules &&
sudo udevadm control --reload-rules &&
sudo udevadm trigger
"@

    Write-Host "teensy_loader_cli installed successfully" -ForegroundColor Green
} else {
    Write-Host "teensy_loader_cli already installed" -ForegroundColor Green
}

# Create directory structure
Write-Host "Creating directories..." -ForegroundColor Yellow
ssh "${User}@${PiHost}" "mkdir -p ${RemotePath}/backend/uploads"

# Copy requirements.txt for venv setup
Write-Host "Copying requirements.txt..." -ForegroundColor Yellow
scp "${ScriptDir}/backend/requirements.txt" "${User}@${PiHost}:${RemotePath}/backend/"

# Create virtual environment
Write-Host "Creating Python virtual environment..." -ForegroundColor Yellow
ssh "${User}@${PiHost}" "cd ${RemotePath} && python3 -m venv venv"

# Install dependencies
Write-Host "Installing Python dependencies..." -ForegroundColor Yellow
ssh "${User}@${PiHost}" "cd ${RemotePath} && ./venv/bin/pip install -r backend/requirements.txt"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Failed to install dependencies" -ForegroundColor Red
    exit 1
}

# Install systemd service
Write-Host "Installing systemd service..." -ForegroundColor Yellow
scp "${ScriptDir}/ledcube.service" "${User}@${PiHost}:/tmp/ledcube.service"
ssh "${User}@${PiHost}" "sudo mv /tmp/ledcube.service /etc/systemd/system/ && sudo systemctl daemon-reload && sudo systemctl enable ledcube"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Failed to install service" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Run .\deploy.ps1 to deploy the backend code"
Write-Host "  2. Access http://${PiHost}:5000/api/status to test"
