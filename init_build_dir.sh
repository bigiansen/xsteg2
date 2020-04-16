rm build -rf
mkdir build

cp scripts/build_wasm_lib_debug.sh \
   build/build_wasm_lib_debug.sh

cp scripts/build_wasm_lib_release.sh \
   build/build_wasm_lib_release.sh

cp scripts/build_cli_exec_debug.sh \
   build/build_cli_exec_debug.sh

cp scripts/build_cli_exec_release.sh \
   build/build_cli_exec_release.sh

chmod u=rwx,g=r,o=r build/build_*