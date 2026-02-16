cmake_minimum_required(VERSION 3.29)

# TODO: Remove this for during attack/handoff
add_compile_definitions(DEBUG)


set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



set(OPTIMIZATION_OPTION -O0)
set(DEBUG_OPTION        -gdwarf-3)


set(SDK_ROOT "${CMAKE_SOURCE_DIR}/pseudo-sdk")

set(CMAKE_C_COMPILER /usr/bin/clang)

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

message(${SDK_ROOT}/source)
# Include SDK Headers
include_directories(
    ${SDK_ROOT}/source
)


####################
# Build monocypher #
####################

add_library(monocypher
    Monocypher/src/monocypher.c
    Monocypher/src/optional/monocypher-ed25519.c
)

target_include_directories(monocypher PUBLIC
    Monocypher/src
)

#####################
# Create Executable #
#####################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c pseudo-sdk/source/*.c)

add_executable(
    hsm
    sysconfig/ti_msp_dl_config.c
    sysconfig/mspm0l2228.c
    ${SRC_FILES}
)

target_include_directories(hsm PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(hsm PRIVATE monocypher)