#!/usr/bin/env bash
set -euo pipefail

# Required inputs
required_scad=(
    "output/scad/left.scad"
    "output/scad/bottom_left.scad"
    "output/scad/right.scad"
    "output/scad/bottom_right.scad"
)

test_keys_scad="output/scad/test_keys.scad"

missing=0
for f in "${required_scad[@]}"; do
    if [[ ! -f "$f" ]]; then
        missing=1
    fi
done

if [[ "$missing" -ne 0 ]]; then
    echo "SCAD files are missing, please generate them with './build/dactyl'" >&2
    exit 1
fi

echo "Making stl, this will take a while..."

mkdir -p output/stl

openscad -o output/stl/left.stl         output/scad/left.scad
openscad -o output/stl/bottom_left.stl  output/scad/bottom_left.scad
openscad -o output/stl/right.stl        output/scad/right.scad
openscad -o output/stl/bottom_right.stl output/scad/bottom_right.scad

if [[ -f "$test_keys_scad" ]]; then
    openscad -o output/stl/test_keys.stl "$optional_scad"
fi

