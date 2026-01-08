# =============================================================================
# NST AI Features Installer for Windows
# =============================================================================
# This script installs the AI-powered features for NST using uv.
# uv ensures correct Python version wheels are downloaded.
#
# Usage:
#   .\install-ai-features.ps1          # Install CPU version (default)
#   .\install-ai-features.ps1 -GPU     # Install GPU version (requires CUDA)
# =============================================================================

param(
    [switch]$GPU = $false
)

$ErrorActionPreference = "Stop"

Write-Host "╔══════════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║           NST AI Features Installer (Windows)                    ║" -ForegroundColor Cyan
Write-Host "╠══════════════════════════════════════════════════════════════════╣" -ForegroundColor Cyan
Write-Host "║  This will install:                                              ║" -ForegroundColor Cyan
Write-Host "║  • EasyOCR        - Text detection from images                   ║" -ForegroundColor Cyan
if ($GPU) {
    Write-Host "║  • PyTorch (GPU)  - AI framework with CUDA support               ║" -ForegroundColor Cyan
} else {
    Write-Host "║  • PyTorch (CPU)  - AI framework (no GPU required)               ║" -ForegroundColor Cyan
}
Write-Host "╚══════════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

# Find NST root directory from script location
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$NSTRoot = Split-Path -Parent $ScriptDir

# Check for bundled Python in Windows installation
# Windows structure: NST-x.x.x-Windows-x64-MSVC/
#   ├── NST.exe
#   ├── python/
#   │   └── Lib/
#   │       └── site-packages/
#   └── scripts/
#       └── install-ai-features.ps1
$PySitePackages = $null
$BundledPyVer = $null

# Primary location: python\Lib\site-packages (Windows embed structure)
$PythonDir = Join-Path $NSTRoot "python"
$SitePackagesPath = Join-Path $PythonDir "Lib\site-packages"

if (Test-Path $SitePackagesPath) {
    $PySitePackages = $SitePackagesPath
    
    # Try to detect Python version from python.exe or python3*.dll
    $PythonDlls = Get-ChildItem -Path $PythonDir -Filter "python3*.dll" -ErrorAction SilentlyContinue
    if ($PythonDlls) {
        # e.g., python312.dll -> 3.12
        $dllName = $PythonDlls[0].BaseName
        if ($dllName -match "python(\d)(\d+)") {
            $BundledPyVer = "$($Matches[1]).$($Matches[2])"
        }
    }
    
    if (-not $BundledPyVer) {
        $BundledPyVer = "3.12"  # Default assumption
    }
}

if (-not $PySitePackages) {
    Write-Host "❌ Error: Cannot find bundled Python in NST." -ForegroundColor Red
    Write-Host "   Looking for site-packages in: $NSTRoot" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "   If you're running from source, create the directory structure first." -ForegroundColor Yellow
    exit 1
}

Write-Host "✓ NST bundled Python: $BundledPyVer" -ForegroundColor Green
Write-Host "✓ Install target: $PySitePackages" -ForegroundColor Green

# Check if uv is installed
$UvCmd = $null

# Check common uv locations on Windows
$UvPaths = @(
    "uv",  # In PATH
    "$env:USERPROFILE\.local\bin\uv.exe",
    "$env:USERPROFILE\.cargo\bin\uv.exe",
    "$env:LOCALAPPDATA\uv\uv.exe"
)

foreach ($uvPath in $UvPaths) {
    try {
        if ($uvPath -eq "uv") {
            $null = Get-Command uv -ErrorAction Stop
            $UvCmd = "uv"
            break
        } elseif (Test-Path $uvPath) {
            $UvCmd = $uvPath
            break
        }
    } catch {
        continue
    }
}

if (-not $UvCmd) {
    Write-Host ""
    Write-Host "📦 Installing uv (fast Python package manager)..." -ForegroundColor Yellow
    
    # Install uv using PowerShell installer
    try {
        Invoke-RestMethod https://astral.sh/uv/install.ps1 | Invoke-Expression
    } catch {
        Write-Host "❌ Error: Failed to download uv installer." -ForegroundColor Red
        Write-Host "   Please install uv manually: https://docs.astral.sh/uv/getting-started/installation/" -ForegroundColor Yellow
        exit 1
    }
    
    # Refresh PATH
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
    
    # Try to find uv again
    $UvPaths = @(
        "$env:USERPROFILE\.local\bin\uv.exe",
        "$env:USERPROFILE\.cargo\bin\uv.exe",
        "$env:LOCALAPPDATA\uv\uv.exe"
    )
    
    foreach ($uvPath in $UvPaths) {
        if (Test-Path $uvPath) {
            $UvCmd = $uvPath
            break
        }
    }
    
    if (-not $UvCmd) {
        Write-Host "❌ Error: Failed to install uv." -ForegroundColor Red
        exit 1
    }
}

Write-Host "✓ Using uv: $UvCmd" -ForegroundColor Green

# Create temp venv with correct Python version
$TempVenv = Join-Path $env:TEMP "nst-install-venv"
Write-Host ""
Write-Host "📦 Setting up Python $BundledPyVer environment..." -ForegroundColor Yellow

# Clean up any previous venv
if (Test-Path $TempVenv) {
    Remove-Item -Recurse -Force $TempVenv
}

# Create venv with uv
try {
    & $UvCmd venv --python $BundledPyVer $TempVenv 2>$null
} catch {
    Write-Host "Installing Python $BundledPyVer..." -ForegroundColor Yellow
    & $UvCmd python install $BundledPyVer
    & $UvCmd venv --python $BundledPyVer $TempVenv
}

# Get pip path in venv
$VenvPip = Join-Path $TempVenv "Scripts\pip.exe"
$VenvPython = Join-Path $TempVenv "Scripts\python.exe"

# Install pip in the venv
Write-Host "Installing pip in temporary environment..." -ForegroundColor Yellow
& $UvCmd pip install --python $VenvPython pip

Write-Host "✓ Python $BundledPyVer environment ready" -ForegroundColor Green

# Ask for confirmation
Write-Host ""
if ($GPU) {
    Write-Host "This will download PyTorch GPU (~2GB) and EasyOCR." -ForegroundColor Yellow
} else {
    Write-Host "This will download PyTorch CPU (~200MB) and EasyOCR." -ForegroundColor Yellow
}
Write-Host "Estimated time: 5-15 minutes depending on internet speed." -ForegroundColor Yellow
Write-Host ""

$response = Read-Host "Install AI features now? [Y/n]"
if ($response -match "^[Nn]$") {
    Write-Host "Installation cancelled." -ForegroundColor Yellow
    if (Test-Path $TempVenv) {
        Remove-Item -Recurse -Force $TempVenv
    }
    exit 0
}

Write-Host ""
Write-Host "📦 Installing packages..." -ForegroundColor Yellow
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Gray

# Install PyTorch
Write-Host ""
if ($GPU) {
    Write-Host "[1/2] Installing PyTorch (GPU with CUDA)..." -ForegroundColor Cyan
    & $VenvPip install `
        --target="$PySitePackages" `
        --upgrade --no-user `
        torch torchvision
} else {
    Write-Host "[1/2] Installing PyTorch (CPU)..." -ForegroundColor Cyan
    & $VenvPip install `
        --target="$PySitePackages" `
        --upgrade --no-user `
        torch torchvision --index-url https://download.pytorch.org/whl/cpu
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error: Failed to install PyTorch." -ForegroundColor Red
    exit 1
}

# Install EasyOCR (without deps to prevent overwriting torch CPU with CUDA)
Write-Host ""
Write-Host "[2/2] Installing EasyOCR..." -ForegroundColor Cyan

# First install easyocr without dependencies
& $VenvPip install `
    --target="$PySitePackages" `
    --upgrade --no-user --no-deps `
    easyocr

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error: Failed to install EasyOCR." -ForegroundColor Red
    exit 1
}

# Then install remaining easyocr dependencies (excluding torch/torchvision which we already have)
Write-Host "Installing EasyOCR dependencies..." -ForegroundColor Yellow
& $VenvPip install `
    --target="$PySitePackages" `
    --upgrade --no-user `
    opencv-python-headless scipy numpy Pillow scikit-image python-bidi PyYAML Shapely pyclipper ninja

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Error: Failed to install EasyOCR dependencies." -ForegroundColor Red
    exit 1
}

# Cleanup
if (Test-Path $TempVenv) {
    Remove-Item -Recurse -Force $TempVenv
}

Write-Host ""
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Gray
Write-Host ""
Write-Host "╔══════════════════════════════════════════════════════════════════╗" -ForegroundColor Green
Write-Host "║  ✅ Installation Complete!                                       ║" -ForegroundColor Green
Write-Host "╠══════════════════════════════════════════════════════════════════╣" -ForegroundColor Green
Write-Host "║  Please restart NST to enable AI features.                       ║" -ForegroundColor Green
Write-Host "║                                                                  ║" -ForegroundColor Green
Write-Host "║  Note: First OCR run will download language models (~100MB).    ║" -ForegroundColor Green
Write-Host "╚══════════════════════════════════════════════════════════════════╝" -ForegroundColor Green
Write-Host ""
