#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
# INIT SCRIPT
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

# -- C++ STANDARD WITHOUT EXTENSIONS --
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message("CURRENT COMPILER ID: ${CMAKE_CXX_COMPILER_ID}")

# -- COMPILER --
IF (CMAKE_CXX_COMPILER_ID MATCHES "CLANG|Clang|clang")
    message("XSTEG2_COMPILER_CLANG ON")
    SET(XSTEG2_COMPILER_CLANG ON)
ENDIF()

IF (CMAKE_CXX_COMPILER_ID MATCHES "MSVC|msvc")
    message("XSTEG2_COMPILER_MSVC ON")
    SET(XSTEG2_COMPILER_MSVC ON)
ENDIF()

IF (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Gnu|gnu")
    SET(XSTEG2_COMPILER_GCC ON)
ENDIF()

IF (CMAKE_CXX_COMPILER_ID MATCHES "Intel|INTEL|intel")
    SET(XSTEG2_COMPILER_INTEL ON)
ENDIF()

# -- ARCH --
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    set (XSTEG2_ARCH_X86 ON)
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM|aarch64|AARCH64")
    set (XSTEG2_ARCH_ARM ON)
endif ()

# -- UTILS --
macro(XSTEG2_SET_SHARED_LIB_OUTPUT_DIR DIR)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY 			${DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG 	${DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE 	${DIR})
endmacro()

macro(XSTEG2_SET_STATIC_LIB_OUTPUT_DIR DIR)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 			${DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 	${DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 	${DIR})
endmacro()

macro(XSTEG2_SET_EXECUTABLE_OUTPUT_DIR DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 			${DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${DIR})
endmacro()

function(XSTEG2_INIT_GIT_SUBMODULES)
    find_package(Git QUIET)

    if(NOT GIT_FOUND)
        message(FATAL_ERROR "Could not initialize git submodules. Git executable not found!")
    endif()

    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
        message(FATAL ERROR "Project is missing the '.git' directory")
    endif()

    message(STATUS "-- INITIALIZING GIT SUBMODULES --")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SM_INIT_OK
    )

    if(NOT GIT_SM_INIT_OK EQUAL "0")
        message(FATAL_ERROR "Failure to initialize git submodules! Error code: ${GIT_SM_INIT_OK}")
    endif()

    message(STATUS "-- INITIALIZING GIT SUBMODULES -- [OK]")

endfunction()