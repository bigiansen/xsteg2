cmake_minimum_required (VERSION "3.7.0")

project(xsteg2 LANGUAGES CXX C)

include(cmake/xsteg2.cmake)

option(XSTEG2_BUILD_CLI_EXECUTABLE "Build CLI executable" On)
option(XSTEG2_BUILD_TESTS "Build tests" On)

XSTEG2_SET_SHARED_LIB_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/shared)
XSTEG2_SET_STATIC_LIB_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/static)
XSTEG2_SET_EXECUTABLE_OUTPUT_DIR(${PROJECT_BINARY_DIR}/output/lib/xsteg2)

# Enable SSE/AVX code generation on X86 platforms
# This should go in a toolchain, but will remain here for the moment
if(XSTEG2_ARCH_X86)
    if(MSVC)
        add_definitions(/arch:AVX2)
    else()
        add_definitions(-mavx2)
    endif()
endif()

# Enable NEON code generation on ARM platforms, if enabled by the XSTEG2_ARM_ENABLE_NEON option
# This should go in a toolchain, but will remain here for the moment
if(XSTEG2_ARCH_ARM AND XSTEG2_ARM_ENABLE_NEON)
    message("########## ARM NEON ENABLED! ###########")
    add_definitions(-mfpu=neon -mfloat-abi=hard -march=armv7-a -funsafe-math-optimizations)
	add_compile_definitions(XSTEG2_ARM_NEON)
endif()

XSTEG2_INIT_GIT_SUBMODULES()

if(XSTEG2_BUILD_TESTS)
    add_subdirectory("test")
    add_subdirectory("third-party")
endif()

add_subdirectory("lib")

if(XSTEG2_BUILD_CLI_EXECUTABLE)
    add_subdirectory("bin")
endif()