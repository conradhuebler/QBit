#!/bin/bash
set -ex
git clone -b 5.9.3 https://github.com/qt/qtcharts.git
cd qtcharts
qmake CONFIG+=release
make 
