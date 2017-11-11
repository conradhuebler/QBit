echo on

SET project_dir="%cd%"

echo Building QBit...
git submodule update --init --recursive
git pull --recurse-submodules
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\mingw-w64\i686-6.3.0-posix-dwarf-rt_v5-rev1 -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

echo Running tests...

echo Packaging...
cd %project_dir%\build
windeployqt QBit.exe

echo Copying project files for archival...
copy "%project_dir%\README.md" "QBit\README.md"
copy "%project_dir%\LICENSE" "QBit\LICENSE.txt"

echo Packaging portable archive...
7z a QBit_%TAG_NAME%_windows.zip QBit
