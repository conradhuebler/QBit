# QBit - Incomplete Features Implementation Proposal

**Analysis Date**: 2025-11-18
**Analyzed Commit**: 925af7d (Qt6 port)
**Status**: Proposal for implementation

---

## Executive Summary

This document outlines unfinished implementations found in the QBit codebase and provides concrete proposals for completing or removing them.

### Priority Overview

| Priority | Feature | Estimated Effort | User Impact |
|----------|---------|------------------|-------------|
| 🔴 HIGH | Version System | 2-3 hours | Medium - Affects --version flag |
| 🔴 HIGH | About/License Dialogs | 3-4 hours | High - Basic UX expectation |
| 🔴 HIGH | JEOL/FID Loader Decision | 1-8 hours | High - Core functionality |
| 🟡 MEDIUM | Qt5→Qt6 Description | 5 minutes | Low - Cosmetic |
| 🟡 MEDIUM | Dead Code Cleanup | 1 hour | Low - Code quality |
| 🟢 LOW | Chart Animations | 5 minutes | Low - Visual polish |
| 🟢 LOW | Widget Size Limits | 5 minutes | Low - UI tweak |

---

## Detailed Proposals

### 1. Version System Reactivation 🔴 HIGH

**Location**: `CMakeLists.txt:56-64`, `src/main.cpp:67`

**Current State**:
- Template files `version.h.in` and `global_config.h.in` don't exist
- Version string never set in application
- `--version` flag doesn't show useful info

**Proposal**: Implement lightweight version system without template files

**Implementation**:

```cmake
# CMakeLists.txt - Add after line 3
project(QBit
    VERSION 0.1.0  # Semantic versioning
    LANGUAGES CXX)

# Add compile definitions (after line 53)
add_definitions("-DQBIT_VERSION=\"${PROJECT_VERSION}\"")
add_definitions("-DQBIT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}")
add_definitions("-DQBIT_VERSION_MINOR=${PROJECT_VERSION_MINOR}")
add_definitions("-DQBIT_VERSION_PATCH=${PROJECT_VERSION_PATCH}")
```

```cpp
// src/main.cpp:67 - Uncomment and use
app.setApplicationVersion(QBIT_VERSION);
```

**Benefits**:
- Simple, no template files needed
- Works with CMake's built-in versioning
- Compatible with CI versioning scheme (`0.1.<run_number>`)

**Risks**: None

**Recommendation**: ✅ **IMPLEMENT**

---

### 2. About & License Dialogs 🔴 HIGH

**Location**: `src/gui/qbit.cpp:73-83`, `src/gui/qbit.h:65`

**Current State**:
- Menu actions declared but commented out
- No implementation exists
- Slot functions don't exist

**Proposal**: Implement basic dialogs using Qt's standard components

**Implementation**:

**Step 1**: Create dialog files
- `src/gui/dialogs/aboutdialog.h`
- `src/gui/dialogs/aboutdialog.cpp`

**Step 2**: Implement simple dialogs
```cpp
// About Dialog - Show version, author, description
class AboutDialog : public QDialog {
    // Display app name, version, copyright, website
    // Use QLabel with rich text
};

// License Dialog - Show GPL v3
class LicenseDialog : public QDialog {
    // Display LICENSE.md content
    // Use QTextBrowser for scrollable text
};
```

**Step 3**: Add slots to QBit class
```cpp
// qbit.h
private slots:
    void showAbout();
    void showLicense();
    void showSettings(); // Placeholder for future
```

**Step 4**: Uncomment menu actions, connect to new slots

**Benefits**:
- Professional appearance
- User can see version info
- GPL compliance (show license)

**Risks**: None

**Recommendation**: ✅ **IMPLEMENT BASIC VERSION**

**Future Enhancement**: Settings dialog (complex, defer for now)

---

### 3. JEOL File Loader 🔴 HIGH

**Location**: `src/core/filehandler.cpp:200-232`

**Current State**:
- Function exists but is non-functional
- Core logic commented out
- Memory leak (new without delete)
- Returns `true` without loading anything

**Proposal Options**:

#### Option A: **Remove Completely** ⭐ RECOMMENDED
**Reasoning**:
- No working implementation exists
- JEOL format is vendor-specific (less common than Bruker)
- No evidence users need this (no GitHub issues)
- Reduces maintenance burden

**Implementation**:
```cpp
bool SpectrumLoader::loadJEOLFile()
{
    // JEOL format not currently supported
    qWarning() << "JEOL file format is not supported in this version";
    return false;
}
```

Add to documentation:
```
Supported formats:
- Bruker 1r files (binary)
- TopSpin exports (ASCII)
- ACDLabs exports (ASCII)
- DPT files (CSV)

Not supported:
- JEOL files (future consideration)
```

#### Option B: **Implement Properly**
**Effort**: 6-8 hours + testing
**Requirements**:
- Research JEOL binary format specification
- Find/create test data
- Implement binary parser
- Test thoroughly

**Recommendation**: ⚠️ **OPTION A - REMOVE**
(Can revisit if users request it)

---

### 4. FID File Loader 🔴 HIGH

**Location**: `src/core/filehandler.cpp:234-319`

**Current State**:
- Returns `false` immediately
- ~85 lines of FFT code commented out
- Was working at some point (code is detailed)

**Analysis of Commented Code**:
- Uses kissfft library (already in dependencies ✅)
- Reads Bruker FID binary files
- Performs FFT to convert time→frequency domain
- Reads acquisition parameters from `acqus` file
- Creates spectrum object

**Proposal Options**:

#### Option A: **Reactivate the Code** ⭐ RECOMMENDED
**Reasoning**:
- FID processing is CORE to NMR analysis
- Code exists and looks complete
- kissfft dependency already included
- Likely commented out for debugging, never re-enabled

**Implementation**:
1. Remove `return false;`
2. Uncomment all code
3. Review for Qt6 compatibility
4. Test with sample FID files
5. Fix any bugs

**Effort**: 2-3 hours (mostly testing)

#### Option B: **Remove Completely**
**Reasoning**: FID is important, this would be wrong

**Recommendation**: ✅ **OPTION A - REACTIVATE**

**Action Items**:
1. Uncomment code
2. Clean up debug statements
3. Add error handling
4. Test with Bruker FID data
5. Document in README

---

### 5. Qt5 → Qt6 Description Fix 🟡 MEDIUM

**Location**: `src/main.cpp:70`

**Current State**:
```cpp
parser.setApplicationDescription("A Open Source Qt5 based NMR Spectra Software, based on libpeakpick.");
```

**Proposal**: Simple string update

**Implementation**:
```cpp
parser.setApplicationDescription("An Open Source Qt6 based NMR Spectra Software, based on libpeakpick.");
```

**Changes**:
- "A" → "An" (grammar)
- "Qt5" → "Qt6" (accuracy)

**Effort**: 1 minute

**Recommendation**: ✅ **IMPLEMENT IMMEDIATELY**

---

### 6. Dead Code Cleanup 🟡 MEDIUM

**Location**: Multiple files

**Current State**: Old threading code, debug statements commented out

**Proposal**: Remove completely

**Files to clean**:

**src/gui/qbit.cpp:106**
```cpp
// REMOVE:
//m_files_widget->setMaximumWidth(200);
```

**src/gui/widgets/multispecwidget.cpp:56**
```cpp
// REMOVE:
//m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
```

**src/gui/widgets/multispecwidget.cpp:404-405, 476-480**
```cpp
// REMOVE old threading code:
//    m_peak_list[index_peak[i]] = parameter(0+i*6);
//qDeleteAll(threads);
//m_threads->waitForDone();
```

**src/gui/widgets/multispecwidget.cpp:723**
```cpp
// REMOVE:
//     qDebug() << i << Xi << x_min << x_max;
```

**src/gui/widgets/multispecwidget.h:108**
```cpp
// REMOVE:
// qDebug() << QPointF(m_spectrum->X(i), (m_raw->Y(i)*m_scaling) + m_number);
```

**Effort**: 30 minutes

**Recommendation**: ✅ **IMPLEMENT** (improves code readability)

---

### 7. Chart Animations 🟢 LOW

**Location**: `src/gui/widgets/multispecwidget.cpp:56`

**Current State**: Disabled

**Proposal**: Test and potentially re-enable

**Implementation**:
```cpp
// Uncomment if performance is acceptable:
m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
```

**Testing Required**:
- Check performance with large spectra
- Verify animations don't interfere with peak picking
- User preference consideration

**Effort**: 5 minutes + testing

**Recommendation**: ⚠️ **TEST FIRST** - May have been disabled for performance reasons

---

### 8. Widget Size Configuration 🟢 LOW

**Location**: `src/gui/qbit.cpp:106`

**Current State**: Maximum width not set

**Proposal**: Test if constraint is needed

**Implementation**:
```cpp
m_files_widget = new FilesWidget;
m_files_widget->setMaximumWidth(250); // Test different values
```

**Testing Required**:
- Does it improve layout?
- Does it restrict too much on small screens?

**Effort**: 10 minutes

**Recommendation**: ⚠️ **USER TESTING REQUIRED**

---

## Implementation Plan

### Phase 1: Quick Wins (1-2 hours) ✅ DO FIRST

**Priority: HIGH - Low risk, immediate value**

1. ✅ Fix Qt5→Qt6 description (1 min)
2. ✅ Clean up dead code (30 min)
3. ✅ Reactivate version system (30 min)
4. ✅ Document changes in commit

**Deliverable**: Clean, accurate codebase

---

### Phase 2: Core Functionality (3-5 hours) ⚠️ REQUIRES TESTING

**Priority: HIGH - Moderate risk, high value**

5. ✅ Reactivate FID loader
   - Uncomment code (5 min)
   - Test with sample data (1-2 hours)
   - Fix bugs (1-2 hours)
   - Document (30 min)

6. ✅ Handle JEOL loader
   - Replace with error message (10 min)
   - Document limitation (10 min)

**Deliverable**: FID files work, JEOL documented as unsupported

---

### Phase 3: User Experience (3-4 hours) 🎨 NEW FEATURES

**Priority: MEDIUM - New code, high UX value**

7. ✅ Implement About Dialog
   - Create dialog class (1 hour)
   - Add to menu (30 min)
   - Test (30 min)

8. ✅ Implement License Dialog
   - Create dialog class (1 hour)
   - Add to menu (30 min)
   - Test (30 min)

**Deliverable**: Professional help menu

---

### Phase 4: Optional Polish (30 min) 🎨 OPTIONAL

**Priority: LOW - Nice to have**

9. ⚠️ Test chart animations
10. ⚠️ Test widget sizing

**Deliverable**: UI refinements

---

## Testing Requirements

### For Each Change:

**Build Test**:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

**Functionality Test**:
- Launch application
- Test affected feature
- Check for crashes/errors
- Verify expected behavior

**Regression Test**:
- Load various file types
- Peak picking still works
- Fit functionality intact
- No new warnings

### Specific Tests:

**FID Loader**:
- Obtain Bruker FID test file
- Verify FFT produces valid spectrum
- Check parameter reading (acqus file)
- Compare with reference software

**Version System**:
```bash
./QBit --version  # Should show "0.1.0"
```

**About Dialogs**:
- Open each dialog
- Verify all info displays
- Check for layout issues
- Test on different screen sizes

---

## Risk Assessment

| Change | Risk Level | Mitigation |
|--------|-----------|------------|
| Version system | 🟢 LOW | Simple defines, well-tested pattern |
| Qt5→Qt6 string | 🟢 LOW | No code change, just string |
| Dead code removal | 🟢 LOW | Only removes comments |
| FID reactivation | 🟡 MEDIUM | Extensive testing required |
| JEOL removal | 🟢 LOW | Returns false (no change in behavior) |
| About dialogs | 🟡 MEDIUM | New code, needs testing |
| Animations | 🟢 LOW | Easy to revert |

---

## Success Criteria

### Phase 1 Success:
- ✅ Build succeeds without warnings
- ✅ No regression in existing features
- ✅ Version shows correctly
- ✅ Code is cleaner (no commented dead code)

### Phase 2 Success:
- ✅ FID files load successfully
- ✅ FFT produces valid spectra
- ✅ Error message for JEOL files is clear
- ✅ No crashes with invalid files

### Phase 3 Success:
- ✅ About dialog shows correct information
- ✅ License displays full GPL text
- ✅ Dialogs are accessible from menu
- ✅ Professional appearance

---

## Rollback Plan

For each phase, create a git commit. If issues arise:

```bash
# Rollback specific commit
git revert <commit-hash>

# Or rollback to before changes
git reset --hard <safe-commit>
```

**Branching Strategy**:
- Create feature branches for risky changes
- Merge only after testing
- Keep main branch stable

---

## Documentation Updates Required

### README.md:
- Update supported file formats
- Note JEOL limitation
- Update version info

### CLAUDE.md:
- Remove issues from "incomplete features"
- Add to "recent improvements"
- Update architecture diagrams if needed

### Changelog:
Add entries:
```
## [0.1.0] - 2025-11-18

### Added
- Version system reactivated
- About and License dialogs
- FID file loading support

### Fixed
- Updated Qt5 to Qt6 in description
- Cleaned up dead code comments

### Removed
- JEOL file loader stub (not implemented)
```

---

## Conclusion

**Recommended Immediate Actions**:
1. ✅ Implement Phase 1 (Quick Wins) - 1-2 hours
2. ✅ Implement Phase 2 (FID Loader) - 3-5 hours
3. ⚠️ Consider Phase 3 (Dialogs) - 3-4 hours
4. ⏸️ Defer Phase 4 (Polish) - Optional

**Total Effort**: 7-11 hours for complete implementation

**Highest Value**: FID loader reactivation (core NMR functionality)

**Lowest Risk**: Quick wins in Phase 1

**Decision Points**:
- JEOL: Remove vs. implement (recommend: remove)
- FID: Reactivate vs. remove (recommend: reactivate)
- Dialogs: Implement vs. defer (recommend: implement)
- Animations: Enable vs. keep disabled (recommend: test first)

---

**Next Steps**: Await approval to proceed with implementation phases.
