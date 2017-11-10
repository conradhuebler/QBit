#!/bin/bash
set -ex
git clone -b 5.9.3 https://github.com/qt/qtcharts.git
qmake CONFIG+=release
qmake CONFIG+=static
make 
make install
