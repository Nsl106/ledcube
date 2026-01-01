# LED Cube Dashboard - Deploy Script
# Copies backend code and restarts service

param(
    [string]$PiHost = "ledcube.local",
    [string]$User = "ledcube",
    [string]$RemotePath = "/home/ledcube/ledcube-dashboard",
    [switch]$NoRestart
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BackendDir = Join-Path $ScriptDir "dashboard\backend"

Write-Host "=== LED Cube Dashboard Deploy ===" -ForegroundColor Cyan
Write-Host "Target: ${User}@${PiHost}:${RemotePath}"
Write-Host ""

# Check backend directory exists
if (-not (Test-Path $BackendDir)) {
    Write-Host "Error: backend directory not found at $BackendDir" -ForegroundColor Red
    exit 1
}

# Copy backend files
Write-Host "Copying backend files..." -ForegroundColor Yellow
scp "${BackendDir}/app.py" "${BackendDir}/requirements.txt" "${User}@${PiHost}:${RemotePath}/backend/"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Failed to copy files" -ForegroundColor Red
    exit 1
}

# Restart service
if (-not $NoRestart) {
    Write-Host "Restarting service..." -ForegroundColor Yellow
    ssh "${User}@${PiHost}" "sudo systemctl restart ledcube"

    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: Failed to restart service. Run setup.ps1 first?" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "Deploy complete!" -ForegroundColor Green
