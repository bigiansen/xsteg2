#!/bin/bash

emcmake cmake -DXSTEG2_BUILD_WASM_LIBRARY=ON -DCMAKE_BUILD_TYPE=DEBUG .. && \
emmake make -j