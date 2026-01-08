# =============================================================================
# NST AI Features Installer for Windows
# =============================================================================
# Usage:
#   .\install-ai-features.ps1          # Install CPU version (default)
#   .\install-ai-features.ps1 -GPU     # Install GPU version (requires CUDA)
# =============================================================================

param(
    [switch]$GPU = $false
)

$ErrorActionPreference = "Stop"

Write-Host "------------------------------------------------------------------" -ForegroundColor Cyan
Write-Host "            NST AI Features Installer (Windows)                   " -ForegroundColor Cyan
Write-Host "------------------------------------------------------------------" -ForegroundColor Cyan
if ($GPU) {
    Write-Host "   Installing: PyTorch (GPU) and EasyOCR" -ForegroundColor Cyan
} else {
    Write-Host "   Installing: PyTorch (CPU) and EasyOCR" -ForegroundColor Cyan
}
Write-Host "------------------------------------------------------------------" -ForegroundColor Cyan
Write-Host ""

# -----------------------------------------------------------------------------
# Path Detection Block (Robust)
# -----------------------------------------------------------------------------
$ScriptDir = $PSScriptRoot
if (-not $ScriptDir) {
    $ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
}
if (-not $ScriptDir) {
    $ScriptDir = (Get-Location).Path
}

$NSTRoot = Split-Path -Parent $ScriptDir
if (-not $NSTRoot) {
    Write-Error "Could not determine installation folder. Ensure you are running this from the 'scripts' folder."
    exit 1
}

# -----------------------------------------------------------------------------
# Check for Bundled Python
# -----------------------------------------------------------------------------
$PySitePackages = $null
$BundledPyVer = "3.12" 

$PythonDir = Join-Path $NSTRoot "python"
$SitePackagesPath = Join-Path $PythonDir "Lib\site-packages"

if (Test-Path $SitePackagesPath) {
    $PySitePackages = $SitePackagesPath
    
    # Try to detect version
    $PythonDlls = Get-ChildItem -Path $PythonDir -Filter "python3*.dll" -ErrorAction SilentlyContinue
    if ($PythonDlls) {
        $dllName = $PythonDlls[0].BaseName
        if ($dllName -match "python(\d)(\d+)") {
            $BundledPyVer = "$($Matches[1]).$($Matches[2])"
        }
    }
}

if (-not $PySitePackages) {
    Write-Host " Error: Cannot find bundled Python in NST." -ForegroundColor Red
    Write-Host "    Looking for site-packages in: $NSTRoot" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "    If you are running from source, create the directory structure first." -ForegroundColor Yellow
    exit 1
}

Write-Host " NST bundled Python: $BundledPyVer" -ForegroundColor Green
Write-Host " Install target: $PySitePackages" -ForegroundColor Green

# -----------------------------------------------------------------------------
# Check / Install UV
# -----------------------------------------------------------------------------
$UvCmd = $null
$UvPaths = @("uv", "$env:USERPROFILE\.local\bin\uv.exe", "$env:USERPROFILE\.cargo\bin\uv.exe", "$env:LOCALAPPDATA\uv\uv.exe")

foreach ($uvPath in $UvPaths) {
    try {
        if ($uvPath -eq "uv") {
            $null = Get-Command uv -ErrorAction Stop
            $UvCmd = "uv"; break
        } elseif (Test-Path $uvPath) {
            $UvCmd = $uvPath; break
        }
    } catch { continue }
}

if (-not $UvCmd) {
    Write-Host ""
    Write-Host "Installing uv (fast Python package manager)..." -ForegroundColor Yellow
    
    try {
        Invoke-RestMethod https://astral.sh/uv/install.ps1 | Invoke-Expression
    } catch {
        Write-Host " Error: Failed to download uv installer." -ForegroundColor Red
        exit 1
    }
    
    # --- FIX: SIMPLIFIED PATH UPDATE ---
    # We break this into steps to prevent parser errors
    $machinePath = [System.Environment]::GetEnvironmentVariable("Path", [System.EnvironmentVariableTarget]::Machine)
    $userPath    = [System.Environment]::GetEnvironmentVariable("Path", [System.EnvironmentVariableTarget]::User)
    $env:Path    = $machinePath + ";" + $userPath
    # -----------------------------------
    
    foreach ($uvPath in $UvPaths) {
        if (Test-Path $uvPath) { $UvCmd = $uvPath; break }
    }
    
    if (-not $UvCmd) {
        Write-Host " Error: Failed to install uv." -ForegroundColor Red
        exit 1
    }
}

Write-Host " Using uv: $UvCmd" -ForegroundColor Green

# -----------------------------------------------------------------------------
# Create Virtual Environment
# -----------------------------------------------------------------------------
$TempVenv = Join-Path $env:TEMP "nst-install-venv"
Write-Host ""
Write-Host " Setting up Python $BundledPyVer environment..." -ForegroundColor Yellow

if (Test-Path $TempVenv) { Remove-Item -Recurse -Force $TempVenv }

try {
    & $UvCmd venv --python $BundledPyVer $TempVenv 2>$null
} catch {
    Write-Host "Installing Python $BundledPyVer..." -ForegroundColor Yellow
    & $UvCmd python install $BundledPyVer
    & $UvCmd venv --python $BundledPyVer $TempVenv
}

$VenvPip = Join-Path $TempVenv "Scripts\pip.exe"
$VenvPython = Join-Path $TempVenv "Scripts\python.exe"

Write-Host "Installing pip in temporary environment..." -ForegroundColor Yellow
& $UvCmd pip install --python $VenvPython pip

# -----------------------------------------------------------------------------
# Install Packages
# -----------------------------------------------------------------------------
Write-Host ""
if ($GPU) {
    Write-Host "This will download PyTorch GPU (~2GB) and EasyOCR." -ForegroundColor Yellow
} else {
    Write-Host "This will download PyTorch CPU (~200MB) and EasyOCR." -ForegroundColor Yellow
}
Write-Host ""

$response = Read-Host "Install AI features now? [Y/n]"
if ($response -match "^[Nn]$") {
    Write-Host "Installation cancelled." -ForegroundColor Yellow
    if (Test-Path $TempVenv) { Remove-Item -Recurse -Force $TempVenv }
    exit 0
}

Write-Host " Installing packages..." -ForegroundColor Yellow

# 1. Install PyTorch
if ($GPU) {
    Write-Host "[1/2] Installing PyTorch (GPU)..." -ForegroundColor Cyan
    & $VenvPip install --target="$PySitePackages" --upgrade --no-user torch torchvision
} else {
    Write-Host "[1/2] Installing PyTorch (CPU)..." -ForegroundColor Cyan
    & $VenvPip install --target="$PySitePackages" --upgrade --no-user torch torchvision --index-url https://download.pytorch.org/whl/cpu
}

if ($LASTEXITCODE -ne 0) {
    Write-Host " Error: Failed to install PyTorch." -ForegroundColor Red
    exit 1
}

# 2. Install EasyOCR
Write-Host "[2/2] Installing EasyOCR..." -ForegroundColor Cyan
& $VenvPip install --target="$PySitePackages" --upgrade --no-user --no-deps easyocr

if ($LASTEXITCODE -ne 0) {
    Write-Host " Error: Failed to install EasyOCR." -ForegroundColor Red
    exit 1
}

# 3. Install EasyOCR Dependencies
Write-Host "Installing dependencies..." -ForegroundColor Yellow
& $VenvPip install --target="$PySitePackages" --upgrade --no-user opencv-python-headless scipy numpy Pillow scikit-image python-bidi PyYAML Shapely pyclipper ninja

if ($LASTEXITCODE -ne 0) {
    Write-Host " Error: Failed to install dependencies." -ForegroundColor Red
    exit 1
}

if (Test-Path $TempVenv) { Remove-Item -Recurse -Force $TempVenv }

Write-Host "Installation Complete"