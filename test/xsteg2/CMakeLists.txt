set(XSTEG2_TESTS_SOURCES
    src/main.cpp
    src/steganographer.cpp
)

add_executable(xsteg2_tests ${XSTEG2_TESTS_SOURCES})
target_link_libraries(xsteg2_tests xsteg2 Catch2::Catch2)