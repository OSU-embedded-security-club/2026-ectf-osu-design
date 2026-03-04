cmake_minimum_required(VERSION 3.29)

# TODO: Remove this for during attack/handoff
add_compile_definitions(DEBUG)


set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)

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
set(TI_LLVM "$ENV{TI_LLVM_ROOT}")

set(CMAKE_C_COMPILER ${TI_LLVM}/bin/tiarmclang)

project(hsm LANGUAGES C)

##########################
# Compiler Configuration #
##########################

# Compiler options
add_compile_options(
    ${PROCESSOR_OPTIONS}
    # ${OPTIMIZATION_OPTION}
    ${LTO_OPTION}
    # ${DEBUG_OPTION}
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
    ${CMAKE_SOURCE_DIR}/firmware.cmd
)

link_directories(
    ${TI_LLVM}/lib
    ${SDK_ROOT}/source
)

link_libraries(
    ${SDK_ROOT}/source/ti/driverlib/lib/ticlang/m0p/mspm0l122x_l222x/driverlib.a
)

####################
# Build monocypher #
####################

add_library(monocypher
    Monocypher/src/monocypher.c
    # Monocypher/src/optional/monocypher-ed25519.c
)

target_include_directories(monocypher PUBLIC
    Monocypher/src
)

target_compile_options(monocypher PRIVATE
    -O2
)

#####################
# Create Executable #
#####################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c)

add_executable(
    hsm
    sysconfig/ti_msp_dl_config.c
    sysconfig/mspm0l2228.c
    ${SRC_FILES}
)

target_include_directories(hsm PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(hsm PRIVATE
    -O0
)

target_link_libraries(hsm PRIVATE monocypher)

set_target_properties(hsm PROPERTIES SUFFIX .elf)

add_custom_command(TARGET hsm POST_BUILD
    COMMAND ${TI_LLVM}/bin/tiarmobjcopy -O binary hsm.elf hsm.bin
)

###########
# Linting #
###########

file(GLOB_RECURSE LINT_FILES CONFIGURE_DEPENDS src/*.h src/*.c)
add_custom_target(lint ${TI_LLVM}/bin/tiarmclang-tidy -p ${CMAKE_CURRENT_BINARY_DIR} ${LINT_FILES})
add_custom_target(lint-ci ${TI_LLVM}/bin/tiarmclang-tidy -p ${CMAKE_CURRENT_BINARY_DIR} --warnings-as-errors=* ${LINT_FILES})

##############
# Build Docs #
##############

find_package(Doxygen)

if(Doxygen_FOUND)
    set(DOXYGEN_PROJECT_NAME "OSU Mitre 2026")

    set(DOXYGEN_EXCLUDE
        ${CMAKE_SOURCE_DIR}/../docs/doxygen-awesome-css
        ${CMAKE_SOURCE_DIR}/../docs/.gitignore
    )

    set(DOXYGEN_HTML_EXTRA_STYLESHEET
        docs/doxygen-awesome-css/doxygen-awesome.css
        docs/doxygen-awesome-css/doxygen-awesome-sidebar-only.css
    )

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_SOURCE_DIR}/../docs/index.md)

    set(DOXYGEN_PLANTUML_JAR_PATH /usr/share/java/plantuml.jar)
    set(DOXYGEN_PLANTUMLFILE_DIRS ${CMAKE_SOURCE_DIR}/../docs/plantuml)

    set(DOXYGEN_DOTFILE_DIRS ${CMAKE_SOURCE_DIR}/../docs/dotfiles)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_INTERACTIVE_SVG YES)

    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_CLASS_DIAGRAMS YES)
    set(DOXYGEN_TYPEDEF_HIDES_STRUCT YES)
    set(DOXYGEN_UML_LOOK YES)

    set(DOXYGEN_EXTRACT_ALL YES)

    # Remove __attribute__ from doxygen parsing
    set(DOXYGEN_ENABLE_PREPROCESSING YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_PREDEFINED "__attribute__(x)=")

    doxygen_add_docs(docs # Target
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/../docs
    )
endif()
