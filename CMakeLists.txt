cmake_minimum_required(VERSION 3.29)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROCESSOR_OPTIONS
    -D__MSPM0L2228__
    -march=thumbv6m
    -mcpu=cortex-m0plus
    -mfloat-abi=soft
    -mlittle-endian
    -mthumb
)

set(OPTIMIZATION_OPTION -O0)
set(LTO_OPTION          -flto)
set(DEBUG_OPTION        -gdwarf-3)


set(SDK_ROOT "${CMAKE_SOURCE_DIR}/mspm0-sdk")
set(TI_LLVM "${CMAKE_SOURCE_DIR}/ti-llvm")

set(CMAKE_C_COMPILER ${TI_LLVM}/bin/tiarmclang)

project(hsm LANGUAGES C)

##########################
# Compiler Configuration #
##########################

# Compiler options
add_compile_options(
    ${PROCESSOR_OPTIONS}
    ${OPTIMIZATION_OPTION}
    ${LTO_OPTION}
    ${DEBUG_OPTION}
)

# Include SDK Headers
include_directories(
    ${SDK_ROOT}/source
    ${SDK_ROOT}/source/third_party/CMSIS/Core/Include
)

########################
# Linker Configuration #
########################

add_link_options(
    ${LTO_OPTION}            # Required when compiling AND linking
    LINKER:--reread_libs
    LINKER:--warn_sections
    LINKER:--rom_model
    ${CMAKE_SOURCE_DIR}/firmware.ld
)

link_directories(
    ${TI_LLVM}/lib
    ${SDK_ROOT}/source
)

link_libraries(
    ${SDK_ROOT}/source/ti/driverlib/lib/ticlang/m0p/mspm0l122x_l222x/driverlib.a
)

#####################
# Create Executable #
#####################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c)

add_executable(
    hsm
    ${SRC_FILES}
)

##############
# Build Docs #
##############

find_package(Doxygen)

if(Doxygen_FOUND)
    set(DOXYGEN_PROJECT_NAME "OSU Mitre 2026")

    set(DOXYGEN_EXCLUDE
        ${CMAKE_SOURCE_DIR}/docs/doxygen-awesome-css
        ${CMAKE_SOURCE_DIR}/docs/.gitignore
    )

    set(DOXYGEN_HTML_EXTRA_STYLESHEET
        docs/doxygen-awesome-css/doxygen-awesome.css
        docs/doxygen-awesome-css/doxygen-awesome-sidebar-only.css
    )

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_SOURCE_DIR}/docs/index.md)

    set(DOXYGEN_EXTRACT_ALL YES)

    doxygen_add_docs(docs # Target
        ./src
        ./docs
    )
endif()