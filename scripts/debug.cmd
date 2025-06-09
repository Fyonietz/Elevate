cd ..
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cd .\build
make
@REM cd ..
@REM cd .\scripts
@REM .\start.cmd