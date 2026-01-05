# NST Distribution Strategy

## 🎯 Goal
To provide a seamless experience for **General Users** (Ease of Use) while enabling **Developers** (Flexibility & Performance) to leverage their hardware.

---

## 1. General Users (Consumer Edition)
**Profile:** Just wants to translate games. Doesn't know what Python or CUDA is.
**Priority:** Stability > Ease of Use > File Size > Performance.

### ✅ Strategy: "All-in-One Bundle" (AppImage)
We distribute a single **AppImage** that contains **everything** needed to run.

*   **Structure:**
    *   **Qt Application:** The compiled C++ executable.
    *   **Python Runtime:** A fully isolated Python 3.12 environment bundled inside the AppImage.
    *   **AI Dependencies:** Pre-installed `torch` (CPU), `easyocr`, `sentence-transformers` inside the bundle.
*   **User Experience:**
    1.  Download `NST-x.x.x.AppImage`.
    2.  `chmod +x` (or Right Click -> Allow Execution).
    3.  Double click to run.
*   **Pros:**
    *   Zero setup. Works on Ubuntu, Fedora, Arch, Steam Deck out of the box.
    *   No "DLL Hell" or version conflicts with system Python.
*   **Cons:**
    *   **Large File Size:** Expect ~800MB - 1.2GB due to AI libraries.
    *   **CPU Only:** By default, we ship CPU versions of PyTorch to ensure compatibility. GPU users won't get acceleration unless we ship a massive CUDA bundle (3GB+).

### 🚀 Implementation Status
*   [x] `requirements.txt` created.
*   [x] CI/CD (`release-linux.yml`) configured to bundle Python + AI Deps.

---

## 2. Developers & Power Users (Pro Edition)
**Profile:** Wants to modify code, define custom rules, or use **GPU Acceleration** (CUDA/ROCm) for faster OCR.
**Priority:** Performance > Flexibility > Setup Time.

### ✅ Strategy: "Bring Your Own Python" (Source or Configurable)
Developers should run the application using their **System Configuration**, allowing them to use their own GPU-optimized libraries.

### Method A: Running from Source (Recommended)
This is the standard Dev workflow.
1.  **Clone Repo:** `git clone ...`
2.  **Install Deps:** `pip install -r requirements.txt` (User can install `torch-cuda` here!).
3.  **Build:** Open in Qt Creator or VS Code -> Build & Run.
4.  **Result:** The app uses the system's `python3`, detecting the user's GPU automatically.

### Method B: Power User AppImage Override (Planned)
Allow Power Users to use the convenient AppImage but force it to use their System Python (with GPU support).

*   **Logic:**
    *   Standard Run: `./NST.AppImage` -> Uses Internal CPU Python.
    *   Power Run: `NST_PYTHON_HOME=/usr/bin/python3 ./NST.AppImage` -> Uses External GPU Python.
    
*   **Implementation Needed:**
    *   Modify `main.cpp` to check for `NST_PYTHON_HOME` environment variable before loading the bundled paths.

---

## 📋 Summary Table

| Feature | General User (AppImage default) | Developer / Power User |
| :--- | :--- | :--- |
| **Delivery** | Single `.AppImage` file | Source Code / Git |
| **Setup** | None (Click & Run) | `pip install` + Build |
| **Python** | Bundled (Isolated) | System Python (Customizable) |
| **AI Speed** | CPU (Standard) | GPU (Fast - if Configured) |
| **Updates** | Download new AppImage | `git pull` |
