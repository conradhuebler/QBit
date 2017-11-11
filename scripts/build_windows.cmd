echo on

SET project_dir="%cd%"

echo Building QBit...

mkdir build
cd build
cmake .. -G %makefiles% -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="c:/dev/install"
if not %platform%==mingw (nmake) else (mingw32-make)
if not %platform%==mingw (nmake install) else (mingw32-make install)

echo Running tests...

echo Packaging...
cd %project_dir%\build\windows\msvc\x86_64\release\
windeployqt QBit.exe

echo Copying project files for archival...
copy "%project_dir%\README.md" "QBit\README.md"
copy "%project_dir%\LICENSE" "QBit\LICENSE.txt"

echo Packaging portable archive...
7z a QBit_%TAG_NAME%_windows_x86_64_portable.zip QBit
