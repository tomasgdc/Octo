cd ..
mkdir build
cd build

cmake -DINTR_FINAL_BUILD:BOOL=ON -G "Visual Studio 16 2019" -A x64 ..

cd ..\Scripts
timeout 2
