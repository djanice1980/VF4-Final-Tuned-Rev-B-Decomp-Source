#!/usr/bin/env bash
# Build the SuperH4_vf4ft language variant and install it as a Ghidra user
# extension (nothing under /opt/ghidra changes). Re-run after editing
# anything in SuperH4_vf4ft/; Ghidra must be closed.
#
# What the variant changes, and why: SuperH4_vf4ft/data/languages/*
# (search for "vf4ft"). Source files are derived from Ghidra's SuperH4
# module, Apache-2.0 (SuperH4_vf4ft/LICENSE.txt).
set -euo pipefail
here=$(cd "$(dirname "$0")" && pwd)
ghidra=/opt/ghidra
ver=$(awk -F= '/^application.version=/ {print $2}' "$ghidra/Ghidra/application.properties")
rel=$(awk -F= '/^application.release.name=/ {print $2}' "$ghidra/Ghidra/application.properties")
dest="$HOME/.config/ghidra/ghidra_${ver}_${rel}/Extensions/SuperH4_vf4ft"

"$ghidra/support/sleigh" "$here/SuperH4_vf4ft/data/languages/SuperH4_le.slaspec"
sed -i "s/^version=.*/version=$ver/" "$here/SuperH4_vf4ft/extension.properties"
rm -rf "$dest"
mkdir -p "$(dirname "$dest")"
cp -r "$here/SuperH4_vf4ft" "$dest"
echo "installed SuperH4:LE:32:vf4ft -> $dest"
