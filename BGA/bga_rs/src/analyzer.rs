//! Core analyzer types and traits

use serde::{Deserialize, Serialize};
use std::path::Path;

/// Output from a game analyzer
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AnalyzerOutput {
    /// MIME type of the payload (e.g., "application/json")
    pub format: String,
    /// The actual payload data (typically JSON)
    pub payload: String,
    /// Error message if analysis failed
    #[serde(skip_serializing_if = "Option::is_none")]
    pub error_message: Option<String>,
}

impl AnalyzerOutput {
    pub fn success(payload: String) -> Self {
        Self {
            format: "application/json".to_string(),
            payload,
            error_message: None,
        }
    }

    pub fn error(message: impl Into<String>) -> Self {
        Self {
            format: "application/json".to_string(),
            payload: String::new(),
            error_message: Some(message.into()),
        }
    }
}

/// A single text entry extracted from a game
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TextEntry {
    /// Original source text
    pub source: String,
    /// File path where the text was found
    pub path: String,
    /// Key path within the file (e.g., "events[0].list[1].parameters[0]")
    pub key: String,
    /// Translated text (populated during save operation)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub text: Option<String>,
}

/// Trait for game analyzers
pub trait GameAnalyzer: Send + Sync {
    /// Analyze a game project and extract translatable strings
    fn analyze(&self, input_path: &Path) -> AnalyzerOutput;

    /// Save translated texts back to the game files
    fn save(&self, texts: &[TextEntry]) -> Result<(), String>;

    /// Check if this analyzer supports script editing
    fn can_edit_script(&self) -> bool {
        false
    }

    /// Get the path to the main script file
    fn get_script_path(&self, _project_path: &Path) -> Option<String> {
        None
    }

    /// Get the script target function name
    fn get_script_target(&self) -> Option<String> {
        None
    }
}
