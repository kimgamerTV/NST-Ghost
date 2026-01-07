    # NST (Novelty Translation Tool)

NST is a desktop application for translating video games. It is built with C++ and the Qt framework.

## Features

*   **Game Engine Support:** Supports translation for games made with RPG Maker and Unity engines.
*   **Translation Interface:** Provides a user-friendly interface for editing and managing translations.
*   **Extensible:** Designed to be extensible with support for new game engines and translation services.

## Getting Started

To build NST from source, you will need:

*   A C++20 compatible compiler
*   CMake 3.16 or later
*   Qt 6 or later

Build instructions:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Image Translation (Optional)

The Image Translation feature uses AI-powered OCR to detect and translate text in screenshots.

### Installing AI Features

Run the included installer script (downloads ~1GB):

```bash
# From extracted AppImage folder:
./usr/bin/scripts/install-ai-features.sh
```

The script automatically installs packages into NST's bundled Python, so it works regardless of your system's Python version.

> **Note:** Without these packages, the app will run in "MOCK mode" for image translation.

## Contributing

Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file for details on how to contribute to this project.

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.
