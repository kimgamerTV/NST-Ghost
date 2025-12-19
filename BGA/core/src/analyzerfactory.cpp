#include "core/analyzerfactory.h"
#include "core/bga_rust_bridge.h"

// Legacy C++ implementations (kept for fallback if needed)
// #include "core/engines/rpgm/rpganalyzer.h"
// #include "core/engines/unity/unityanalyzer.h"
// #include "core/engines/renpy/renpyanalyzer.h"

namespace core {

std::unique_ptr<IGameAnalyzer> createAnalyzer(const QString &engineName)
{
    // Use Rust backend for all engines
    return createRustAnalyzer(engineName);
}

QStringList availableAnalyzers()
{
    // Get available analyzers from Rust library
    return availableRustAnalyzers();
}

} // namespace core
