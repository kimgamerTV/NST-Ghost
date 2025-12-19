//! Ren'Py Analyzer
//!
//! Extracts and saves translatable strings from Ren'Py games.

use crate::analyzer::{AnalyzerOutput, GameAnalyzer, TextEntry};
use once_cell::sync::Lazy;
use regex::Regex;
use serde_json::json;
use std::fs;
use std::io::{BufRead, BufReader, Write};
use std::path::Path;
use std::process::Command;
use walkdir::WalkDir;

// Regex patterns for Ren'Py dialogue
static DIALOG_PATTERN: Lazy<Regex> =
    Lazy::new(|| Regex::new(r#"^\s*(?:[a-zA-Z_]\w*\s+)?"([^"]+)""#).unwrap());

static MENU_PATTERN: Lazy<Regex> =
    Lazy::new(|| Regex::new(r#"^\s*"([^"]+)"\s*:"#).unwrap());

pub struct RenpyAnalyzer;

impl RenpyAnalyzer {
    pub fn new() -> Self {
        Self
    }

    /// Check if unrpyc is available and return the command to use
    fn find_unrpyc() -> Option<Vec<String>> {
        // Try python3 -m unrpyc
        let output = Command::new("python3")
            .args(["-m", "unrpyc", "--help"])
            .output();

        if let Ok(out) = output {
            if out.status.success() {
                return Some(vec![
                    "python3".to_string(),
                    "-m".to_string(),
                    "unrpyc".to_string(),
                ]);
            }
        }

        // Try unrpyc directly
        let output = Command::new("unrpyc").arg("--help").output();

        if let Ok(out) = output {
            if out.status.success() {
                return Some(vec!["unrpyc".to_string()]);
            }
        }

        None
    }

    /// Decompile .rpyc files to .rpy
    fn decompile_rpyc(dir: &Path, unrpyc_cmd: &[String]) -> Result<(), String> {
        for entry in WalkDir::new(dir)
            .max_depth(1)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            if entry.file_type().is_file() {
                if let Some(ext) = entry.path().extension() {
                    if ext == "rpyc" {
                        let rpyc_path = entry.path();
                        let rpy_path = rpyc_path.with_extension("rpy");

                        // Skip if .rpy already exists
                        if rpy_path.exists() {
                            continue;
                        }

                        // Run decompiler
                        let mut cmd = Command::new(&unrpyc_cmd[0]);
                        for arg in &unrpyc_cmd[1..] {
                            cmd.arg(arg);
                        }
                        cmd.arg(rpyc_path);

                        let output = cmd.output().map_err(|e| format!("Failed to run unrpyc: {}", e))?;

                        if !output.status.success() {
                            return Err(format!(
                                "Decompile failed: {}",
                                String::from_utf8_lossy(&output.stderr)
                            ));
                        }
                    }
                }
            }
        }

        Ok(())
    }

    /// Extract strings from .rpy files
    fn extract_from_rpy(path: &Path) -> Vec<TextEntry> {
        let mut entries = Vec::new();

        let file = match fs::File::open(path) {
            Ok(f) => f,
            Err(_) => return entries,
        };

        let reader = BufReader::new(file);
        let file_name = path
            .file_name()
            .map(|s| s.to_string_lossy().to_string())
            .unwrap_or_default();

        for (line_num, line_result) in reader.lines().enumerate() {
            let line = match line_result {
                Ok(l) => l,
                Err(_) => continue,
            };

            // Try dialog pattern first, then menu pattern
            let text = DIALOG_PATTERN
                .captures(&line)
                .or_else(|| MENU_PATTERN.captures(&line))
                .and_then(|caps| caps.get(1))
                .map(|m| m.as_str().to_string());

            if let Some(text) = text {
                if !text.trim().is_empty() {
                    entries.push(TextEntry {
                        source: text.clone(),
                        path: file_name.clone(),
                        key: format!("line:{}", line_num + 1),
                        text: None,
                    });
                }
            }
        }

        entries
    }
}

impl Default for RenpyAnalyzer {
    fn default() -> Self {
        Self::new()
    }
}

impl GameAnalyzer for RenpyAnalyzer {
    fn analyze(&self, input_path: &Path) -> AnalyzerOutput {
        if !input_path.exists() {
            return AnalyzerOutput::error("Path does not exist");
        }

        // Check for .rpyc files and decompile if necessary
        let has_rpyc = WalkDir::new(input_path)
            .max_depth(1)
            .into_iter()
            .filter_map(|e| e.ok())
            .any(|e| {
                e.file_type().is_file()
                    && e.path().extension().map(|ext| ext == "rpyc").unwrap_or(false)
            });

        if has_rpyc {
            if let Some(unrpyc_cmd) = Self::find_unrpyc() {
                if let Err(e) = Self::decompile_rpyc(input_path, &unrpyc_cmd) {
                    return AnalyzerOutput::error(e);
                }
            } else {
                return AnalyzerOutput::error(
                    "unrpyc not found. Install: pip install unrpyc-ng",
                );
            }
        }

        // Extract from .rpy files
        let mut all_entries = Vec::new();

        for entry in WalkDir::new(input_path)
            .max_depth(1)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            if entry.file_type().is_file() {
                if let Some(ext) = entry.path().extension() {
                    if ext == "rpy" {
                        let entries = Self::extract_from_rpy(entry.path());
                        all_entries.extend(entries);
                    }
                }
            }
        }

        if all_entries.is_empty() {
            return AnalyzerOutput::error("No .rpy files found");
        }

        let result = json!({
            "engine": "renpy",
            "source": input_path.to_string_lossy(),
            "strings": all_entries,
        });

        AnalyzerOutput::success(serde_json::to_string_pretty(&result).unwrap_or_default())
    }

    fn save(&self, texts: &[TextEntry]) -> Result<(), String> {
        // For Ren'Py, we generate a translation file rather than modifying originals
        let output_path = "translations.rpy";

        let mut file = fs::File::create(output_path)
            .map_err(|e| format!("Failed to create output file: {}", e))?;

        for entry in texts {
            if let Some(translation) = &entry.text {
                if !translation.is_empty() {
                    writeln!(file, "translate None:")
                        .map_err(|e| format!("Write error: {}", e))?;
                    writeln!(file, "    old \"{}\"", entry.source)
                        .map_err(|e| format!("Write error: {}", e))?;
                    writeln!(file, "    new \"{}\"", translation)
                        .map_err(|e| format!("Write error: {}", e))?;
                    writeln!(file)
                        .map_err(|e| format!("Write error: {}", e))?;
                }
            }
        }

        Ok(())
    }
}
