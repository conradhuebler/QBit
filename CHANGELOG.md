# Changelog

All notable changes to QBit will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2025-11-18

### Added
- **Version System**: Semantic versioning (0.1.0) implemented via CMake
  - Version displayed in About dialog
  - Version accessible via `--version` command-line flag
  - Git commit information embedded in builds (branch, hash, date)
- **About Dialog**: Professional dialog showing application information
  - Displays version, git info, author, and repository link
  - Lists all dependencies (libpeakpick, Eigen, kissfft, CuteChart)
  - Accessible from toolbar "About QBit" button
- **License Dialog**: GPL v3 license information display
  - Shows full license text or summary
  - Attempts to load LICENSE.md from various locations
  - Accessible from toolbar "License Info" button
- **FID File Processing**: Reactivated Bruker FID file loader
  - FFT (Fast Fourier Transform) processing for time-domain data
  - Reads acquisition parameters from Bruker `acqus` file (SW_h, TD, O1)
  - Time-domain to frequency-domain conversion
  - Proper error handling for missing parameter files
- **File Format Support Documentation**: Clear documentation of supported formats
  - Bruker 1r files (binary NMR data)
  - Bruker FID files (with FFT processing)
  - TopSpin ASCII exports
  - ACDLabs ASCII exports
  - DPT CSV files

### Changed
- **Qt6 Migration**: Updated from Qt5 to Qt6.1+
  - Application description updated to reflect Qt6
  - Fixed grammar: "A Open Source" → "An Open Source"
  - All Qt includes and dependencies updated
- **CMakeLists.txt**: Project version updated from 0.0.1 to 0.1.0
- **Compiler Requirements**: Updated to C++17 (from C++14)
- **JEOL Loader**: Replaced non-functional implementation with proper error message
  - Clear user feedback about unsupported format
  - Suggests alternative supported formats
  - Removed broken code that could cause memory leaks

### Fixed
- **Dead Code Cleanup**: Removed all commented-out code blocks
  - Removed old threading code comments in multispecwidget.cpp
  - Removed debug output comments in multispecwidget.h
  - Removed unused widget configuration in qbit.cpp
  - Improved code readability and maintainability
- **Error Messages**: Improved error handling and user feedback
  - FID loader reports missing `acqus` file with warning
  - JEOL loader provides helpful error message instead of failing silently

### Improved
- **Documentation**: Comprehensive updates
  - README.md completely rewritten with detailed usage guide
  - CLAUDE.md created for AI assistant development guidance
  - INCOMPLETE_FEATURES_PROPOSAL.md documenting analysis and fixes
  - Added keyboard shortcuts documentation
  - Added troubleshooting and contribution guidelines

### Technical Details
- Build system requires CMake 3.21+
- Tested with GCC 6.3+, Clang 4.0+, MSVC 2019+
- OpenMP support for parallel processing (Unix systems)
- Qt 6.1+ with Charts module required

## [Unreleased]

### Planned
- Settings/Preferences dialog
- Automated test suite
- Additional file format support (JEOL if requested)
- Enhanced peak fitting algorithms
- Batch processing capabilities
- Export functionality improvements

---

## Historical Changes

### Pre-0.1.0 Development

**Recent Commits**:
- `925af7d` - Port to Qt6
- `aef86e7` - Add redistributable
- `ee5f377` - Static linking experiments
- `be1ae49` - Restore VS 16 (2019)
- `35e3c2a` - Remove Travis and AppVeyor CI

**Previous CI Systems**:
- Migrated from Travis CI to GitHub Actions
- Removed AppVeyor in favor of GitHub Actions
- Nightly builds for Linux, Windows, macOS

---

## Version History

| Version | Date | Major Changes |
|---------|------|---------------|
| 0.1.0 | 2025-11-18 | First semantic versioning release, Qt6 migration, FID loader, About dialogs |
| 0.0.1 | Pre-2025 | Initial development version |

---

## Notes

- **Breaking Changes**: Qt6 migration may require users to update Qt installation
- **Dependencies**: All dependencies fetched automatically via git submodules
- **Platform Support**: Linux (primary), Windows, macOS

For detailed development information, see [CLAUDE.md](CLAUDE.md).
For user documentation, see [README.md](README.md).
