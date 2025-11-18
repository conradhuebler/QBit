Linux and Windows Build: [![Build Status](https://github.com/conradhuebler/QBit/workflows/AutomaticBuild/badge.svg)]

# QBit

**Version 0.1.0** - An Open Source Qt6-based NMR Spectra Viewer and Analysis Tool

QBit provides visualization, peak fitting, and deconvolution capabilities for Nuclear Magnetic Resonance (NMR) spectroscopy data.

## Features

- **Multiple File Format Support**: Bruker (1r, FID), TopSpin, ACDLabs, DPT
- **FFT Processing**: Time-domain to frequency-domain conversion for FID files
- **Peak Detection**: Automated and manual peak picking
- **Peak Fitting**: Single and multiple peak deconvolution
- **Interactive Visualization**: Zoom, pan, scale with Qt Charts
- **Multi-Spectrum Display**: Load and compare multiple spectra

## Download and Requirements

### Dependencies

QBit automatically fetches the following dependencies as git submodules:

- **Eigen**: Non-linear optimization for peak fitting
- **libpeakpick**: Core peak detection and analysis algorithms
- **CuteChart**: Advanced charting and visualization
- **kissfft**: Fast Fourier Transform for FID processing

### System Requirements

- **CMake**: 3.21 or newer
- **C++ Compiler**: C++17-capable
  - GCC 6.3 or newer
  - Clang 4.0 or newer
  - MSVC 2019 or newer
- **Qt**: 6.1 or newer with Charts module
- **OpenMP**: Optional, for parallel processing (Linux/macOS)

### Supported Platforms

- **Linux**: Ubuntu 20.04+ (primary development platform)
- **Windows**: Windows 7+ (Windows 10+ recommended)
- **macOS**: Latest versions

## Installation

### Clone Repository

**Important**: Use `--recursive` flag to fetch all submodules:

```bash
git clone --recursive https://github.com/conradhuebler/QBit.git
cd QBit
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

### Build from Source

#### Linux / macOS

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./QBit
```

#### Windows (MinGW)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
mingw32-make
QBit.exe
```

#### Windows (MSVC)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
Release\QBit.exe
```

### Pre-built Binaries

Download the latest release from [GitHub Releases](https://github.com/conradhuebler/QBit/releases) for your platform:

- `QBit-nightly-*-x86_64-Linux.tar.gz`
- `QBit-nightly-*-x86_x64-Windows.zip`
- `QBit-nightly-*-macOS.dmg`

## Supported File Formats

### ✅ Fully Supported

| Format | Description | Notes |
|--------|-------------|-------|
| **Bruker 1r** | Binary processed NMR data | Requires complete directory structure |
| **Bruker FID** | Free Induction Decay (raw) | FFT processing included |
| **TopSpin ASCII** | Bruker TopSpin text export | Tab/space delimited |
| **ACDLabs ASCII** | ACD/Labs text export | Proprietary format |
| **DPT CSV** | Comma-separated values | Generic format |

### ❌ Not Supported

- **JEOL files**: Vendor-specific binary format (future consideration)

> **Note**: For Bruker files, provide the complete directory structure including all subdirectories and metadata files (e.g., `acqus` for FID processing).

## Usage Guide

### Loading Files

**Single File**:
1. Click **Open File** in toolbar
2. Select spectrum file
3. File loads into viewer

**Multiple Files**:
1. Click **Open Dir** to scan directory
2. All compatible files appear in file list
3. Double-click file to load, or
4. Select multiple files → click **Open Selected**

**Batch Loading**:
- Load entire directory tree (searches recursively for 1r files)
- All found spectra added to file list for selection

### Display and Navigation

**Zoom**:
- **Left mouse drag**: Draw rectangle to zoom into region
- **Middle mouse click**: Reset zoom to full spectrum
- **Mouse wheel**: Scale spectrum intensity

**Pan**:
- Navigate zoomed spectra with scroll bars

### Peak Analysis

**Method 1: Single Peak Fitting**
1. Zoom to region of interest (recommended)
2. Click **Fit Single Peak** button
3. Double-click on peak to add to fit list
4. Select multiple peaks for simultaneous fitting

**Method 2: Automated Peak Picking + Deconvolution**
1. Adjust peak picking sensitivity (optional)
2. Click **Pick Peaks** button
3. Review detected peaks in peak list
4. Click **Deconvulate** to fit all visible peaks

**Method 3: Manual Region Selection**
1. Double right-click to start rectangle
2. Double right-click again to complete
3. Peaks within rectangle are fitted

> **Tip**: Zoom in to exclude unwanted peaks from deconvolution. Only visible peaks in the current zoom level will be fitted.

### Peak List

The peak list shows:
- Peak position (ppm or Hz)
- Peak intensity
- Peak width (FWHM)
- Integration area

Export peak data for further analysis.

## Advanced Features

### FID Processing

QBit can directly process Bruker FID (Free Induction Decay) files:

1. Load FID file from Bruker directory
2. Reads acquisition parameters from `acqus` file
3. Performs FFT (Fast Fourier Transform)
4. Displays frequency-domain spectrum

Parameters extracted:
- `SW_h`: Spectral width (Hz)
- `TD`: Time domain size
- `O1`: Transmitter offset

### Multi-Spectrum Comparison

Load multiple spectra simultaneously:
- Overlaid display with automatic color coding
- Individual scaling and offset
- Synchronized zoom across all spectra

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `Ctrl+O` | Open file |
| `Ctrl+D` | Open directory |
| `Ctrl+Q` | Quit |

## Getting Help

### In-Application Help

- **About QBit**: View version, dependencies, and project info
- **License Info**: GNU GPL v3 license details
- **About Qt**: Qt framework information

### Documentation

- [GitHub Repository](https://github.com/conradhuebler/QBit)
- [Issue Tracker](https://github.com/conradhuebler/QBit/issues)
- [CLAUDE.md](CLAUDE.md): Developer documentation for AI assistants

### Reporting Bugs

Found a bug? Please [open an issue](https://github.com/conradhuebler/QBit/issues) with:
- QBit version (`Help → About QBit`)
- Operating system and version
- Steps to reproduce
- Expected vs. actual behavior
- Sample files (if possible)

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Follow existing code style (`.clang-format`)
4. Test on multiple platforms if possible
5. Submit a pull request

See [CLAUDE.md](CLAUDE.md) for detailed development guidelines.

## License

QBit is free software licensed under the **GNU General Public License v3.0**.

```
Copyright (C) 2017 - 2025 Conrad Hübler <Conrad.Huebler@gmx.net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
```

See [LICENSE.md](LICENSE.md) for full license text.

## Acknowledgments

QBit is built on excellent open-source libraries:

- **Qt6**: Cross-platform application framework
- **Eigen**: C++ template library for linear algebra
- **libpeakpick**: Peak detection algorithms
- **kissfft**: Simple FFT implementation
- **CuteChart**: Charting and visualization

## Citation

If you use QBit in your research, please cite:

```
Hübler, C. (2025). QBit: An Open Source NMR Spectra Viewer and Analysis Tool.
GitHub repository: https://github.com/conradhuebler/QBit
```

## Contact

**Author**: Conrad Hübler
**Email**: Conrad.Huebler@gmx.net
**Repository**: https://github.com/conradhuebler/QBit
