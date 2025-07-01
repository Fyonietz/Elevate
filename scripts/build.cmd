cd ..
cmake -S . -B build -G "Ninja"
cmake --build build
cd .\scripts
.\start.cmd