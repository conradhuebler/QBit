echo on

SET project_dir="%cd%"
echo Building SupraFit x64...
git submodule update --init --recursive
git pull --recurse-submodules
mkdir build_x64
cd build_x64
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release  ..
C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin\mingw32-make.exe


echo Packaging...
cd %project_dir%
cd build_x64
mkdir QBit
copy QBit.exe QBit
copy C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin\libgomp-1.dll QBit

cd SupraFit
windeployqt --release --compiler-runtime  qbit.exe

echo Copying project files for archival...
copy "%project_dir%\README.md" "%project_dir%\build_x64\Release\QBit\README.md"
copy "%project_dir%\LICENSE.md" "%project_dir%\build_x64\Release\QBit\LICENSE.md"

echo Packaging portable archive...

cd ..
7z a QBit.zip QBit

cd ..
