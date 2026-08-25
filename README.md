# groot

`groot` is an interactive nuclear spectrum analysis environment built on
CERN ROOT.

The project adds domain-specific histogram classes, canvas interactions, and
analysis helpers for low-energy nuclear physics. Its goal is to make large
multi-dimensional spectra easier to inspect, gate, project, fit, and compare
inside ROOT.

Current development focuses on enhanced 1D and 2D histogram workflows. Longer
term, `groot` is intended to grow toward higher-dimensional spectrum objects,
including cubes and hypercubes, through a more general `GHn`-style interface.

## Why groot?

ROOT already provides the foundation for histogramming, fitting, drawing, and
I/O. `groot` adds the layer needed for day-to-day nuclear spectrum analysis:
interactive markers, keyboard-driven canvas actions, enriched histogram
behavior, and tools that turn common analysis steps into first-class operations.

## Highlights

- Enhanced histogram workflows for nuclear spectrum analysis
- Marker-driven fitting, gating, zooming, projections, and cuts
- Interactive canvases tuned for fast histogram inspection
- Tools for working with large ROOT histograms and multi-dimensional spectra
- Peak fitting and residual inspection from the ROOT prompt or canvas
- Region-of-interest counting on 1D spectra
- 2D projections onto selected X/Y ranges
- Histomatic GUI browser for ROOT files
- Optional live histogram connections
- Runtime customization through `.gintrc`

## Requirements

`groot` currently builds against a custom CERN ROOT installation.

Known requirements:

- CERN ROOT with GUI support
- CMake 3.13 or newer
- A C++ compiler compatible with the ROOT build
- Make or another CMake-supported build tool

`groot` uses ROOT histograms, canvases, fitting, I/O, GUI widgets, and
dictionary generation. ROOT should be built with the corresponding components
enabled.

## Build

Source your ROOT environment first:

```sh
source /path/to/root/bin/thisroot.sh
```

Then build:

```sh
make
```

The default `make` target configures CMake in `build/`, builds `groot`, and
copies the executable to `bin/`.

You can also build directly with CMake:

```sh
cmake -S . -B build
cmake --build build -j4
```

## Packaged ROOT Status

Building against package-manager ROOT installations, such as Homebrew or apt
ROOT, is a project goal but is not yet verified. The current known-good path is
a custom ROOT build with the required GUI and dictionary support enabled.

## Quick Start

Start the interactive prompt:

```sh
./bin/groot
```

Open the GUI with a ROOT file:

```sh
./bin/groot -g data.root
```

<!--
Connect to a live histogram source:

```sh
./bin/groot -g ws://localhost:8080/live/
```
-->

Show command-line help:

```sh
./bin/groot -h
```

## Basic Interaction

The current interactive command list lives in the wiki:

- [Commands](docs/wiki/Commands.md)

## Repository Layout

| Path | Purpose |
| --- | --- |
| `src/` | Main executable |
| `include/` | Public headers |
| `libraries/GRoot/` | Histogram, canvas, marker, ROI, fit, and projection tools |
| `libraries/Histomatic/` | GUI browser |
| `libraries/GCore/` | Options, live connections, and object management |
| `libraries/GApp/` | ROOT interactive application wrapper |
| `libraries/GTools/` | Nuclear and transition helper types |
| `scripts/` | Analysis helper macros and scripts |
| `docs/` | Additional usage notes |

## Documentation

The README is intended as a starting point: what `groot` is, what it needs, and
how to build and launch it. The fuller manual is maintained in the wiki:

- [groot wiki home](docs/wiki/Home.md)
- [Building groot](docs/wiki/Building-groot.md)
- [Commands](docs/wiki/Commands.md)
