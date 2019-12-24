set(XSTEG2_SOURCES
    src/availability.cpp
    src/steganographer.cpp
    src/threshold.cpp
    src/visual_data.cpp
)

FILE(GLOB XSTEG2_HEADERS include/xsteg/*.hpp)

add_library(xsteg2 ${XSTEG2_SOURCES} ${XSTEG2_HEADERS})
target_include_directories(xsteg2 PUBLIC include)
target_link_libraries(xsteg2 lien_image lien_strutils)