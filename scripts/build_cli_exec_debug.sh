#!/bin/bash

cmake -DXSTEG2_BUILD_CLI_EXECUTABLE=ON -DCMAKE_BUILD_TYPE=DEBUG .. && \
make -j