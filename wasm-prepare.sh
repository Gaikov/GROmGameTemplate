rm bin -r -f
cmake \
-DCMAKE_TOOLCHAIN_FILE=~/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
-DCMAKE_C_COMPILER=$(which emcc) \
-DCMAKE_CXX_COMPILER=$(which em++) \
-DCMAKE_LINKER=$(which emcc) \
-DCMAKE_AR=$(which emar) \
-DCMAKE_C_COMPILER_WORKS=1 \
-DCMAKE_CXX_COMPILER_WORKS=1 \
-DWEB_ASM=ON -S ./ -B ./bin -G Ninja
./wasm-build.sh