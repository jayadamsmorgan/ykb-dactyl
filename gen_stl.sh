#!/usr/bin/env bash

echo Making stl, this will take a while...

mkdir -p output/stl

set -x
openscad -o output/stl/left.stl output/scad/left.scad
openscad -o output/stl/bottom_left.stl output/scad/bottom_left.scad
openscad -o output/stl/right.stl output/scad/right.scad
openscad -o output/stl/bottom_right.stl output/scad/bottom_right.scad
