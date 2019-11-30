Linux and Windows Build: [![Build Status](https://github.com/conradhuebler/QBit/workflows/AutomaticBuild/badge.svg)]

# QBit 
A Open Source Qt5 based spectra viewing tool (mainly NMR).

## Download and requirements
git clones automatically Eigen. Eigen is used as non-linear optimimization tool.

## Compiling
To compile QBit you will need CMake 3 or newer and a C++14-capable compiler.

QBit has been successfully compilied with: 
- GCC 6.3 , 7.x and 8.x
- Clang 4.0

on Linux systems and with
- MinGW 5.3

on Windows systems.

> Windows 7 or higher is recommended if Qt is compilied without ICU support.

To obtain QBit use git:
```sh
git clone --recursive git@github.com:conradhuebler/QBit.git
cd QBit
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Short usage

### Loading files

Load NMR spectrum, either exported as txt file via TopSpin or ACDLabs or open directly 1r files. Please provide a complete directory structure for Bruker NMR files. A directory, including all subdirectories can be added and all 1r files be loaded into the list view. Single spectra can be loaded via double click in list, multiple spectra can be loaded by selecting them in the list view and then pushing **Open Selected**

### Display and Zoom

Spectra can be zoom using the left mouse button (`click, drag and drop`), the zoom can be reseted clicking the middle mouse button. Turn the mouse wheele scales the spectra.

### Fitting peaks

Simple peak fitting can be done **Fit Single Peak**, an apropriate zoom is recommended. Double click on the desired peak adds it to the list, multiple selection are possible. 

Multiple peaks can be fitted, if **pick peaks* is done prior to **Deconvulate**. Please keep in mind, that all visible peaks will be fitted. You can zoom in to exclude peaks.

A third possibility is drawing an recangle via double right click around the desired peak.
