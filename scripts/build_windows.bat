echo on

SET project_dir="%cd%"
SET VCInstallDir="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Tools/MSVC/14.23.28105/bin/Hostx64/x64/"
echo Building QBit x64...
git submodule update --init --recursive
git pull --recurse-submodules
mkdir build_x64
cd build_x64
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -MT ..
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

