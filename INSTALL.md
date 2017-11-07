## Download and requirements
git clones automatically Eigen. Eigen is used as non-linear optimimization tool.

## Compiling
To compile QBIt you will need CMake 3 or newer and a C++14-capable compiler.

QBit has been successfully compilied with: 
- gcc 6.3
 

> Windows 7 or higher is recommended if Qt is compilied without ICU support.

```sh
git clone --recursive git@github.com:contra98/QBit.git
cd suprafit
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
