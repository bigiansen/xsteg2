set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -s DISABLE_EXCEPTION_CATCHING=0 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s SAFE_HEAP=1 \
    -s ASSERTIONS=1 \
    --post-js ${CMAKE_CURRENT_SOURCE_DIR}/src/wasm_api.glue.js"
)

# Generate WebIDL glue code
if(NOT EXISTS $ENV{EMSDK})
    message(FATAL "Unable to find emsdk path")
else()
    message("Building WebIDL Glue Code...")
    execute_process(
        COMMAND python $ENV{EMSDK}/upstream/emscripten/tools/webidl_binder.py idl/wasm_api.idl src/wasm_api.glue
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE WASM_GLUE_OK
    )
endif()

add_custom_target(xsteg2_wasm_glue DEPENDS src/wasm_api.glue.cpp)

add_executable(xsteg2_wasm 
    src/wasm_api.cpp
)

add_dependencies(xsteg2_wasm xsteg2_wasm_glue)

target_link_libraries(xsteg2_wasm xsteg2)

target_link_options(xsteg2_wasm PRIVATE "-s ALLOW_MEMORY_GROWTH=1")
target_link_options(xsteg2_wasm PRIVATE "-s DISABLE_EXCEPTION_CATCHING=0")
target_link_options(xsteg2_wasm PRIVATE "-s SAFE_HEAP=1")
target_link_options(xsteg2_wasm PRIVATE "-s ASSERTIONS=1")