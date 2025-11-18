# CLAUDE.md - AI Assistant Guide for QBit

## Project Overview

**QBit** is an open-source Qt6-based NMR (Nuclear Magnetic Resonance) spectra viewing and analysis tool written in C++17. The application provides visualization, peak fitting, and deconvolution capabilities for NMR spectroscopy data.

### Key Information
- **Language**: C++17
- **GUI Framework**: Qt6 (6.1+)
- **Build System**: CMake 3.21+
- **License**: GNU GPL v3
- **Author**: Conrad Hübler
- **Repository**: https://github.com/conradhuebler/QBit
- **Recent Migration**: Ported from Qt5 to Qt6 (commit: 925af7d)

### Supported Platforms
- **Linux**: Primary development platform (Ubuntu 20.04+)
- **Windows**: Windows 7+ (Windows 2019 for CI)
- **macOS**: Latest macOS versions

## Codebase Structure

```
QBit/
├── src/                           # Main source code
│   ├── main.cpp                   # Application entry point
│   ├── core/                      # Core data structures and file handling
│   │   ├── nmrspec.{h,cpp}       # NMR spectrum data structure
│   │   └── filehandler.{h,cpp}   # File I/O operations
│   ├── gui/                       # GUI components
│   │   ├── qbit.{h,cpp}          # Main window class
│   │   ├── dialogs/              # Dialog windows
│   │   │   ├── selectguess.{h,cpp}
│   │   │   └── fitparameter.{h,cpp}
│   │   ├── widgets/              # Custom Qt widgets
│   │   │   ├── multispecwidget.{h,cpp}   # Main spectrum display
│   │   │   ├── peakwidget.{h,cpp}        # Peak visualization
│   │   │   ├── fileswidget.{h,cpp}       # File list management
│   │   │   ├── glfitwidget.{h,cpp}       # Fitting display
│   │   │   ├── glfitlist.{h,cpp}         # Fit results list
│   │   │   ├── logwidget.{h,cpp}         # Log output
│   │   │   └── peakposcallout.{h,cpp}    # Peak position markers
│   │   └── helpers/              # GUI utility functions
│   └── func/                      # Computational functions
│       ├── fit_threaded.{h,cpp}  # Multi-threaded peak fitting
│       └── pick_threaded.{h,cpp} # Multi-threaded peak picking
├── external/                      # Third-party dependencies (git submodules)
│   ├── libpeakpick/              # Peak picking library (+ Eigen)
│   ├── CuteChart/                # Charting library
│   └── kissfft/                  # FFT library
├── scripts/                       # Build and deployment scripts
│   ├── build_unix.sh
│   ├── build_windows.bat
│   ├── build_macOS_github.sh
│   └── deploy_unix_dir.sh
├── .github/workflows/            # CI/CD configuration
│   └── ccpp.yml                  # GitHub Actions workflow
├── icons/                        # Application icons
├── files.qrc                     # Qt resource file
├── CMakeLists.txt               # CMake build configuration
├── .clang-format                # Code formatting rules
├── README.md                    # User documentation
└── INSTALL.md                   # Build instructions
```

### File Count
- **Total C++ files**: 30 source files (.cpp and .h)
- **Core components**: ~10 files
- **GUI components**: ~16 files
- **Functional components**: 4 files

## Architecture and Design Patterns

### Application Architecture

```
┌─────────────────────────────────────────┐
│         QBit (QMainWindow)              │
│  Main application window and controller │
└────────────┬────────────────────────────┘
             │
    ┌────────┴────────┬──────────────────┐
    │                 │                  │
┌───▼───────┐  ┌─────▼─────┐   ┌───────▼────────┐
│  Core     │  │    GUI    │   │  Computational │
│  Layer    │  │   Layer   │   │     Layer      │
└───────────┘  └───────────┘   └────────────────┘
    │              │                    │
    │              │                    │
┌───▼───────┐  ┌──▼────────────┐  ┌───▼────────┐
│ NMRSpec   │  │ MultiSpecWidget│  │ Threaded   │
│           │  │ PeakWidget     │  │ Fitting    │
│FileHandler│  │ FilesWidget    │  │ Picking    │
└───────────┘  └────────────────┘  └────────────┘
```

### Key Design Patterns

1. **Model-View Architecture**
   - `fileHandler`: Manages data model
   - Widgets: Display and interact with data
   - Signals/Slots: Qt's event-driven communication

2. **Multi-threaded Processing**
   - Peak fitting: `fit_threaded.cpp`
   - Peak picking: `pick_threaded.cpp`
   - Keeps GUI responsive during computation

3. **Qt QPointer Pattern**
   - Used throughout for safe widget lifetime management
   - See `src/gui/qbit.h:54-59`

4. **Dock Widget Layout**
   - Flexible, user-customizable interface
   - Files, peaks, fit results, and logs in separate docks

## Build System and Dependencies

### CMake Configuration

**Minimum Requirements:**
- CMake 3.21+
- C++17 compiler
- Qt6 6.1+ with Charts module

**Build Variables:**
- `CMAKE_BUILD_TYPE`: Release/Debug
- `CMAKE_CXX_STANDARD`: Set to 17
- `CMAKE_AUTOMOC`: Enabled (Qt meta-object compilation)
- `CMAKE_AUTORCC`: Enabled (Qt resource compilation)

**Git Integration:**
- Build captures: `GIT_COMMIT_HASH`, `GIT_BRANCH`, `GIT_COMMIT_DATE`
- Embedded in binary via preprocessor definitions

### External Dependencies (Git Submodules)

1. **libpeakpick** (github.com/conradhuebler/libpeakpick)
   - Contains Eigen library for non-linear optimization
   - Core peak detection and fitting algorithms

2. **CuteChart** (github.com/conradhuebler/CuteChart)
   - Custom charting library (formerly part of SupraFit)
   - Spectrum visualization

3. **kissfft** (github.com/mborgerding/kissfft)
   - Fast Fourier Transform implementation
   - Signal processing

**Important**: Always clone with `--recursive` flag:
```bash
git clone --recursive https://github.com/conradhuebler/QBit.git
```

### Compiler Support

**Tested Compilers:**
- GCC 6.3, 7.x, 8.x, 9.x
- Clang 4.0+
- MinGW 5.3+ (Windows)
- MSVC 2019 (Windows)

**Compiler Flags (GCC):**
- Extensive warning flags enabled (see `CMakeLists.txt:75-90`)
- `-Wall -Wextra -pedantic` and many specific warnings
- OpenMP support on Unix systems

### Platform-Specific Configurations

**Linux:**
- OpenMP for parallel processing
- Links: `pthread`, `dl`

**Windows:**
- MSVC: Static runtime linking (`MultiThreaded`)
- Entry point: `mainCRTStartup`
- Win32 executable (no console window)

**macOS:**
- Creates `.app` bundle
- `MACOSX_BUNDLE` property set

## Development Workflow

### Initial Setup

```bash
# Clone repository with submodules
git clone --recursive https://github.com/conradhuebler/QBit.git
cd QBit

# Initialize and update submodules (if not cloned with --recursive)
git submodule init
git submodule update --recursive

# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Run the application
./QBit
```

### Common Development Tasks

#### Building for Different Platforms

**Linux:**
```bash
sh scripts/build_unix.sh
```

**Windows:**
```bash
scripts\build_windows.bat
```

**macOS:**
```bash
sh scripts/build_macOS_github.sh
```

#### Code Formatting

The project uses **clang-format** with WebKit-based style:
- **Indent**: 4 spaces (never tabs)
- **Pointer alignment**: Left (`int* ptr`, not `int *ptr`)
- **Column limit**: No limit (0)
- **Brace style**: WebKit (opening brace on same line, except functions)
- **Standard**: C++11 (though code uses C++17)

**To format code:**
```bash
clang-format -i src/**/*.cpp src/**/*.h
```

#### Updating Submodules

```bash
git submodule update --recursive --remote
```

**Note**: Build scripts automatically checkout `master` branch for all submodules.

### Git Branch Strategy

- **master**: Main development branch
- CI triggers on push/PR to master
- Feature branches should merge via pull requests

### Continuous Integration

**GitHub Actions** (`.github/workflows/ccpp.yml`):
- Triggered on: Push to master, PRs to master
- **Three parallel jobs**: Linux (Ubuntu 20.04), Windows (2019), macOS (latest)
- **Qt Installation**: Uses `jurplel/install-qt-action@v2` with Qt 6.3.x
- **Artifacts**: Creates releases with platform-specific binaries
- **Versioning**: `0.1.<run_number>`
- **Release Management**: Keeps latest 5 releases

**Build Matrix:**
| Platform | OS | Qt Version | Architecture |
|----------|----|-----------:|--------------|
| Linux | Ubuntu 20.04 | 6.3.* | x86_64 |
| Windows | Windows 2019 | 6.3.* | x64 (MinGW) |
| macOS | Latest | 6.3.* | Universal |

## Code Conventions and Guidelines

### Naming Conventions

**Classes:**
- PascalCase: `NMRSpec`, `MultiSpecWidget`, `FileHandler`

**Member Variables:**
- Prefix `m_`: `m_files`, `m_spec_widget`, `m_current_index`

**Functions:**
- PascalCase for public methods: `LoadFile()`, `LoadDir()`
- camelCase acceptable for helpers

**Files:**
- Lowercase: `nmrspec.cpp`, `filehandler.h`
- Match class name in lowercase

### Code Style Guidelines

1. **Include Order** (as per .clang-format):
   - Project headers with priority
   - Standard library headers
   - Third-party headers

2. **Pointer/Reference Alignment**:
   ```cpp
   // Correct
   QString* name;
   const NMRSpec& spec;

   // Incorrect
   QString *name;
   const NMRSpec &spec;
   ```

3. **Indentation**:
   - 4 spaces per level
   - Never use tabs

4. **Braces**:
   - WebKit style: opening brace on same line (except functions)
   ```cpp
   if (condition) {
       // code
   }

   void function()
   {
       // code
   }
   ```

5. **Qt-Specific**:
   - Use `Q_OBJECT` macro for classes with signals/slots
   - Use `QPointer` for widget pointers to prevent dangling references
   - Connect signals/slots in constructor
   - Use Qt containers (`QString`, `QVector`) for Qt objects

### Error Handling and Logging

**Message Handler** (`src/main.cpp:30-53`):
- Custom Qt message handler installed: `qInstallMessageHandler(myMessageOutput)`
- Outputs to stderr with file/line information
- Message types: Debug, Info, Warning, Critical, Fatal

**Debug Mode:**
```cpp
#ifdef _DEBUG
    qDebug() << "Debug output enabled, good fun!";
#endif
```

### Memory Management

1. **Qt Object Ownership**:
   - Parent-child relationship: Qt automatically deletes child objects
   - Use `QPointer` for safe weak references

2. **Manual Memory**:
   - In `NMRSpec`: Manual `new`/`delete` for `PeakPick::spectrum*`
   - Copy constructors and assignment operators implemented

3. **RAII**:
   - Prefer stack allocation when possible
   - Use Qt's smart pointers when needed

## Key Components Deep Dive

### Core Layer

#### NMRSpec (`src/core/nmrspec.{h,cpp}`)
Represents a single NMR spectrum with three data components:
- **Raw spectrum**: Original unprocessed data
- **Processed data**: Transformed/processed spectrum
- **FID (Free Induction Decay)**: Time-domain signal

**Usage:**
```cpp
NMRSpec spec(name, raw_data, processed_data, is_nmr);
spec.setPath(filepath);
PeakPick::spectrum* data = spec.Data();
```

#### FileHandler (`src/core/filehandler.{h,cpp}`)
Manages loading and storing spectrum files:
- Supports TopSpin exports (txt)
- Supports ACDLabs exports
- Bruker `1r` files (direct reading)
- Directory traversal for batch loading

### GUI Layer

#### QBit (`src/gui/qbit.{h,cpp}`)
Main application window:
- **Inherits**: `QMainWindow`
- **Manages**: All child widgets and docks
- **Responsibilities**: Menu actions, file loading, widget coordination

**Key Methods:**
- `LoadFile(QString)`: Load single spectrum file
- `LoadFiles(QStringList)`: Load multiple files
- `LoadSpectrum(int)`: Display spectrum at index

#### MultiSpecWidget (`src/gui/widgets/multispecwidget.{h,cpp}`)
Primary spectrum visualization widget:
- **Size**: ~27KB, largest GUI component
- **Features**:
  - Interactive zoom (left mouse drag)
  - Reset zoom (middle mouse)
  - Scale (mouse wheel)
  - Peak picking and fitting
  - Multiple spectrum overlay

#### Supporting Widgets

**FilesWidget**: File list management
**PeakWidget**: Peak table display
**GLFitWidget**: Fitting results visualization
**GLFitList**: Fit parameters list
**LogWidget**: Application log output
**PeakPosCallout**: Chart annotations for peak positions

### Functional Layer

#### Threaded Operations

**fit_threaded.{h,cpp}**:
- Non-blocking peak fitting
- Uses Eigen for optimization
- Runs in separate thread to keep UI responsive

**pick_threaded.{h,cpp}**:
- Automated peak detection
- Threshold-based or derivative-based algorithms
- Multi-threaded for performance

## Common Tasks for AI Assistants

### Adding a New Widget

1. Create header/source in `src/gui/widgets/`
2. Include in `CMakeLists.txt` under `qbit_gui`
3. Add to main window in `src/gui/qbit.cpp`
4. Connect signals/slots in constructor
5. Add to appropriate dock or layout

Example:
```cpp
// In qbit.h
QPointer<NewWidget> m_new_widget;

// In qbit.cpp constructor
m_new_widget = new NewWidget(this);
m_dock = new QDockWidget("New Widget", this);
m_dock->setWidget(m_new_widget);
addDockWidget(Qt::RightDockWidgetArea, m_dock);
```

### Adding a New File Format

1. Extend `fileHandler` class
2. Add parsing logic in `filehandler.cpp`
3. Create `NMRSpec` objects from parsed data
4. Update file dialog filters if needed

### Modifying Peak Fitting Algorithm

1. Locate algorithm in `src/func/fit_threaded.cpp`
2. Modify optimization parameters (Eigen-based)
3. Update threading/progress reporting
4. Test with various spectrum types

### Adding Menu Actions

1. Add `QAction*` member to `QBit` class
2. Create action in constructor:
   ```cpp
   m_action = new QAction("Name", this);
   connect(m_action, &QAction::triggered, this, &QBit::slotMethod);
   ```
3. Add to menu or toolbar
4. Implement slot method

### Debugging Tips

1. **Enable debug output**:
   - Define `_DEBUG` preprocessor macro
   - Check `src/main.cpp:81-83`

2. **Qt Creator**: Best IDE for Qt development
   - Built-in UI designer
   - Qt signal/slot debugging
   - CMake integration

3. **Common Issues**:
   - Submodules not initialized → Run `git submodule update --init --recursive`
   - Qt version mismatch → Check CMake Qt6 version
   - MOC errors → Clean build directory and rebuild

## Testing and Quality Assurance

### Current State
- **No formal test suite**: Project currently lacks unit/integration tests
- **Manual testing**: Primary QA method
- **CI builds**: Ensures compilation on all platforms

### Testing Workflow for AI Assistants

When making changes:
1. **Build successfully** on target platform
2. **Test manually**:
   - Load sample NMR files
   - Test zoom/pan/scale
   - Run peak picking
   - Perform peak fitting
   - Check export functionality
3. **Cross-platform**: Consider platform-specific behavior
4. **Memory leaks**: Use Valgrind (Linux) or similar tools

### Recommended Test Additions

If adding tests:
1. Create `tests/` directory
2. Use Qt Test framework (`QTest`)
3. Add test target to CMakeLists.txt
4. Test core functionality:
   - File loading
   - Peak detection algorithms
   - Data transformations

## File Formats and Data Handling

### Supported Input Formats

1. **Bruker 1r files**:
   - Binary format
   - Requires complete directory structure
   - Most common NMR data format

2. **TopSpin exports** (txt):
   - Text-based export from Bruker TopSpin
   - Tab or space-delimited

3. **ACDLabs exports**:
   - Text-based format
   - Proprietary format from ACD/Labs software

### Data Processing Pipeline

```
File Load → Parse → NMRSpec Creation → Display
    ↓           ↓           ↓              ↓
Directory   Format      Raw + Data    Chart Render
Scan       Detection    Spectrum      + Peak Marks
```

### FFT Processing
- Uses kissfft library
- Time-domain → Frequency-domain conversion
- Accessible through NMRSpec FID data

## Performance Considerations

1. **Threading**:
   - CPU-intensive operations use worker threads
   - UI remains responsive during computation

2. **OpenMP**:
   - Enabled on Unix systems
   - Parallel loop execution where applicable

3. **Large Files**:
   - Lazy loading considerations
   - Memory management for multiple spectra

4. **Chart Rendering**:
   - Qt Charts used for visualization
   - Hardware acceleration where available

## Security Considerations

### Input Validation
- File path validation needed when loading
- Prevent directory traversal attacks
- Validate file format before parsing

### Resource Limits
- Consider limits on:
  - Number of simultaneously loaded spectra
  - File size limits
  - Memory usage for large datasets

## Recent Improvements (2025-11-18)

### Completed Features

1. **Version System Reactivated** ✅
   - Semantic versioning (0.1.0) implemented via CMake
   - Version displayed in About dialog and --version flag
   - Git information embedded in builds

2. **FID File Loader Reactivated** ✅
   - FFT processing for Bruker FID files fully functional
   - Reads acquisition parameters from `acqus` file
   - Time-domain to frequency-domain conversion working

3. **About and License Dialogs** ✅
   - Professional About dialog with version, author, dependencies
   - License dialog displays GPL v3 information
   - Added to toolbar for easy access

4. **Code Quality Improvements** ✅
   - Removed all commented-out dead code
   - Fixed Qt5→Qt6 description string
   - Improved error messages for unsupported formats

5. **JEOL Loader Cleanup** ✅
   - Non-functional stub replaced with clear error message
   - Documents supported file formats

### File Format Support Status

**Fully Supported:**
- ✅ Bruker 1r files (binary NMR data)
- ✅ Bruker FID files (with FFT processing)
- ✅ TopSpin ASCII exports
- ✅ ACDLabs ASCII exports
- ✅ DPT CSV files

**Not Supported:**
- ❌ JEOL files (vendor-specific format, future consideration)

## Future Development Areas

Based on recent commits and code structure:

1. **Qt6 Migration** ✓ **COMPLETED**
   - Port from Qt5 to Qt6 ✓
   - All functionality verified and working ✓

2. **Potential Improvements**:
   - Add automated test suite
   - Settings/Preferences dialog
   - Improve error handling and user feedback
   - Enhanced peak fitting algorithms
   - Batch processing capabilities
   - Export functionality enhancements
   - JEOL file format support (if requested by users)

3. **Code Modernization**:
   - Adopt more C++17 features
   - Consider smart pointers over raw pointers
   - Add const correctness
   - Improve exception safety

## Resources and Documentation

### Official Resources
- **Repository**: https://github.com/conradhuebler/QBit
- **Issues**: GitHub Issues for bug reports/features
- **License**: LICENSE.md (GPL v3)

### Qt Documentation
- **Qt6 Docs**: https://doc.qt.io/qt-6/
- **Qt Charts**: https://doc.qt.io/qt-6/qtcharts-index.html
- **Qt Widgets**: https://doc.qt.io/qt-6/qtwidgets-index.html

### Related Projects
- **libpeakpick**: Peak detection library
- **CuteChart**: Charting library
- **SupraFit**: Related project from same author

## Contact and Contribution

**Maintainer**: Conrad Hübler <Conrad.Huebler@gmx.net>

### Contributing Guidelines

1. Fork the repository
2. Create feature branch
3. Follow code conventions (use .clang-format)
4. Test on multiple platforms if possible
5. Submit pull request to master
6. CI must pass (Linux, Windows, macOS builds)

### Code Review Focus Areas

When submitting PRs, ensure:
- Code follows .clang-format style
- No compiler warnings introduced
- Qt best practices followed
- Memory management is correct
- Cross-platform compatibility maintained
- Comments explain non-obvious logic

## Troubleshooting Guide

### Build Issues

**Problem**: Submodules not found
```bash
# Solution
git submodule update --init --recursive
```

**Problem**: Qt6 not found
```bash
# Solution: Install Qt6 with Charts module
# Linux: sudo apt install qt6-base-dev qt6-charts-dev
# Or use Qt installer: https://www.qt.io/download
```

**Problem**: CMake version too old
```bash
# Solution: Install CMake 3.21+
# Linux: sudo snap install cmake --classic
```

### Runtime Issues

**Problem**: Application won't start
- Check Qt libraries are in PATH (Windows)
- Use `ldd ./QBit` to check dependencies (Linux)
- Check console output for Qt warnings

**Problem**: Files won't load
- Verify file format is supported
- Check file permissions
- Look for errors in log widget

**Problem**: Crashes during peak fitting
- Check for large spectrum data (memory)
- Verify Eigen library is properly linked
- Enable debug mode for more information

## Appendix: Key File Locations

### Configuration Files
- `CMakeLists.txt`: Build configuration
- `.clang-format`: Code style rules
- `.gitmodules`: Submodule definitions
- `files.qrc`: Qt resources (icons, etc.)

### Entry Points
- `src/main.cpp`: Application entry point (line 56)
- `src/gui/qbit.cpp`: Main window initialization

### Critical Classes
- `src/core/nmrspec.h`: Data structure definition
- `src/gui/qbit.h`: Main window interface
- `src/gui/widgets/multispecwidget.h`: Primary display widget

### Build Scripts
- `scripts/build_unix.sh`: Linux build automation
- `scripts/build_windows.bat`: Windows build automation
- `scripts/deploy_unix_dir.sh`: Linux deployment packaging

### CI/CD
- `.github/workflows/ccpp.yml`: GitHub Actions workflow

---

**Document Version**: 1.0
**Last Updated**: 2025-11-18
**Based on Commit**: 925af7d (Qt6 port)
**For QBit Version**: 0.0.1

---

## Quick Reference Card

### Build Commands
```bash
# Fresh build
git clone --recursive https://github.com/conradhuebler/QBit.git
cd QBit && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release && make

# Rebuild
cd build && make clean && make

# Update submodules
git submodule update --recursive --remote
```

### File Locations Quick Reference
| Component | Path |
|-----------|------|
| Main window | `src/gui/qbit.{h,cpp}` |
| Spectrum data | `src/core/nmrspec.{h,cpp}` |
| File I/O | `src/core/filehandler.{h,cpp}` |
| Peak fitting | `src/func/fit_threaded.{h,cpp}` |
| Main display | `src/gui/widgets/multispecwidget.{h,cpp}` |
| Build config | `CMakeLists.txt` |
| CI/CD | `.github/workflows/ccpp.yml` |

### Code Style Quick Tips
- Indent: 4 spaces
- Pointers: `int* ptr` (left-aligned)
- Braces: WebKit style
- Members: Prefix with `m_`
- Qt includes: Use `<QtModule/QClass>`
- Format: `clang-format -i <file>`

---

*This document is maintained for AI assistants working on the QBit codebase. For user documentation, see README.md.*
