echo on

SET project_dir="%cd%"

echo Set up environment...
set PATH=%QT%\bin\;C:\Qt\Tools\QtCreator\bin\;C:\Qt\QtIFW2.0.1\bin\;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %PLATFORM%


echo Building QBit...

mkdir build
cd build
cmake .. "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_PREFIX_PATH=%QT%"
nmake

echo Running tests...

echo Packaging...
cd %project_dir%\build\windows\msvc\x86_64\release\
windeployqt --qmldir ..\..\..\..\..\src\ QBit.exe

echo Copying project files for archival...
copy "%project_dir%\README.md" "QBit\README.md"
copy "%project_dir%\LICENSE" "QBit\LICENSE.txt"

echo Packaging portable archive...
7z a QBit_%TAG_NAME%_windows_x86_64_portable.zip QBit
