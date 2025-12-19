#ifndef BGA_RUST_BRIDGE_H
#define BGA_RUST_BRIDGE_H

#include "core/gameanalyzer.h"
#include <QString>
#include <QJsonArray>
#include <memory>

// C FFI declarations from Rust library
extern "C" {
    /// Analyze a game project
    /// @param engine Engine name ("rpgm", "unity", "renpy")
    /// @param path Path to the game project
    /// @return JSON string (must be freed with bga_free_string)
    char* bga_analyze(const char* engine, const char* path);

    /// Save translated texts back to game files
    /// @param engine Engine name
    /// @param texts_json JSON array of text entries
    /// @return 0 on success, non-zero on error
    int bga_save(const char* engine, const char* texts_json);

    /// Get available analyzer names
    /// @return JSON array of names (must be freed with bga_free_string)
    char* bga_available_analyzers();

    /// Free a string allocated by the Rust library
    void bga_free_string(char* ptr);

    /// Get the library version
    const char* bga_version();
}

namespace core {

/// C++ wrapper for Rust-based game analyzers
class RustAnalyzerBridge : public IGameAnalyzer {
public:
    explicit RustAnalyzerBridge(const QString& engineName);
    ~RustAnalyzerBridge() override = default;

    /// Analyze the game project at the given path
    AnalyzerOutput analyze(const QString& inputPath) override;

    /// Save translated texts back to the game files
    bool save(const QString& outputPath, const QJsonArray& texts) override;

    /// Check if script editing is supported
    bool canEditScript() const override { return m_engine == "rpgm"; }

    /// Get the script path for the project
    QString getScriptPath(const QString& projectPath) const override;

    /// Get the script target function name
    QString getScriptTarget() const override;

private:
    QString m_engine;
};

/// Create an analyzer using the Rust backend
std::unique_ptr<IGameAnalyzer> createRustAnalyzer(const QString& engineName);

/// Get available analyzers from the Rust library
QStringList availableRustAnalyzers();

} // namespace core

#endif // BGA_RUST_BRIDGE_H
