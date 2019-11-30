echo on

SET project_dir="%cd%"

echo Building QBit x32 and x64...
git submodule update --init --recursive
git pull --recurse-submodules
mkdir build_x64
cd build_x64
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release


echo Packaging...
cd %project_dir%
cd build_x64\Release
mkdir QBit
copy QBit.exe QBit
cd QBit
windeployqt --release QBit.exe

echo Packaging portable archive...
cd ..
7z a QBit_latest_x64_windows.zip QBit

cd ..

mkdir build_x32
cd build_x32
cmake -G "Visual Studio 15 2017 Win32" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release


echo Packaging...
cd %project_dir%
cd build_x32\Release
mkdir QBit
copy QBit.exe QBit
cd QBit
windeployqt --release QBit.exe

echo Packaging portable archive...
cd ..
7z a QBit_latest_x32_windows.zip QBit
