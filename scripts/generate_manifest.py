import os
import json
import hashlib

def generate_manifest(version, release_notes, assets_dir):
    manifest = {
        "version": version,
        "release_notes": release_notes,
        "files": {}
    }

    for root, _, files in os.walk(assets_dir):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            with open(file_path, "rb") as f:
                file_hash = hashlib.sha256(f.read()).hexdigest()
            
            # Construct the download URL
            # This assumes the script is run in the context of a GitHub Action
            repo = os.environ.get("GITHUB_REPOSITORY")
            tag = os.environ.get("GITHUB_REF_NAME")
            url = f"https://github.com/{repo}/releases/download/{tag}/{file_name}"

            manifest["files"][file_name] = {
                "hash": file_hash,
                "url": url
            }

    with open(os.path.join(assets_dir, "manifest.json"), "w") as f:
        json.dump(manifest, f, indent=4)

if __name__ == "__main__":
    version = os.environ.get("GITHUB_REF_NAME").replace("v", "")
    # For release notes, you might want to get them from the git tag message
    # or from a file. For simplicity, we'll use a placeholder here.
    release_notes = "See the release notes on GitHub."
    assets_dir = "artifacts"
    generate_manifest(version, release_notes, assets_dir)
