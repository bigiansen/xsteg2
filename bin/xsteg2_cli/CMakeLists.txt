SET(XSTEG2_CLI_SOURCES
    src/arg_iterator.cpp
    src/main.cpp
)

add_executable(xsteg2_cli ${XSTEG2_CLI_SOURCES})
target_link_libraries(xsteg2_cli xsteg2)