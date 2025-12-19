//! Unity Analyzer
//!
//! Basic file listing for Unity projects (asset/prefab scanning).

use crate::analyzer::{AnalyzerOutput, GameAnalyzer, TextEntry};
use serde_json::json;
use std::path::Path;
use walkdir::WalkDir;

pub struct UnityAnalyzer;

impl UnityAnalyzer {
    pub fn new() -> Self {
        Self
    }
}

impl Default for UnityAnalyzer {
    fn default() -> Self {
        Self::new()
    }
}

impl GameAnalyzer for UnityAnalyzer {
    fn analyze(&self, input_path: &Path) -> AnalyzerOutput {
        let mut entries = Vec::new();

        for entry in WalkDir::new(input_path)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            if entry.file_type().is_file() {
                if let Some(ext) = entry.path().extension() {
                    let ext_str = ext.to_string_lossy().to_lowercase();
                    if ext_str == "asset" || ext_str == "prefab" {
                        entries.push(json!({
                            "path": entry.path().to_string_lossy(),
                            "type": ext_str,
                        }));
                    }
                }
            }
        }

        let result = json!({
            "engine": "unity",
            "source": input_path.to_string_lossy(),
            "entries": entries,
        });

        AnalyzerOutput::success(serde_json::to_string_pretty(&result).unwrap_or_default())
    }

    fn save(&self, _texts: &[TextEntry]) -> Result<(), String> {
        Err("Saving for Unity projects is not yet implemented due to complex asset format.".into())
    }
}
