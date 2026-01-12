# YKB Dactyl

## Build

Clone repository with its submodules:

```bash
git clone https://github.com/jayadamsmorgan/ykb-dactyl --recursive
```
Build:

```bash
meson setup build
meson compile -C build
```

Run:

```bash
./build/dactyl
```
This will generate OpenSCAD files in `output/scad` directory.

## Generate STL

```bash
./gen_stl.sh
```

This will generate STL files with OpenSCAD in `output/stl` directory.
