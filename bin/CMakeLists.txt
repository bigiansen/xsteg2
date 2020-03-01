add_subdirectory(xsteg2_cli)

if(XSTEG2_BUILD_WASM_LIBRARY)
    add_subdirectory(xsteg2_wasm)
endif()