cd ..
cmake -S . -B build
cd .\build
make -j4
cd ..
cd .\scripts
.\start.cmd