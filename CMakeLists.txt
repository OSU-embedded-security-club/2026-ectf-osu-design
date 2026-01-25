cmake_minimum_required(VERSION 3.29)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "BUILD_MODE = '${BUILD_MODE}'")



set(OPTIMIZATION_OPTION -O0)
set(LTO_OPTION          -flto)
set(DEBUG_OPTION        -gdwarf-3)



if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
  set(CMAKE_SYSTEM_NAME Generic)
  set(PROCESSOR_OPTIONS
      -D__MSPM0L2228__
      -march=thumbv6m
      -mcpu=cortex-m0plus
      -mfloat-abi=soft
      -mlittle-endian
      -mthumb
  )

  set(SDK_ROOT "${CMAKE_SOURCE_DIR}/mspm0-sdk")
  message(STATUS "ROOT = '${SDK_ROOT}'")
  set(TI_LLVM "$ENV{TI_LLVM_ROOT}")

  set(CMAKE_C_COMPILER ${TI_LLVM}/bin/tiarmclang)
else()

  add_compile_definitions(SIMU)
  add_compile_options(
    -O0
    -g
    -Wall
    -Wextra
  )


endif()

project(hsm LANGUAGES C)

##########################
# Compiler Configuration #
##########################

if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
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

endif()
########################
# Linker Configuration #
########################

if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
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
endif()

#################
# Build mbedtls #
#################

# Disable testing and programs
set(ENABLE_TESTING off CACHE BOOL "Enable mbedtls testing")
set(ENABLE_PROGRAMS off CACHE BOOL "Enable mbedtls programs")

# Set config files
set(MBEDTLS_CONFIG_FILE ${CMAKE_SOURCE_DIR}/mbedtls-config/mbedtls_config.h
CACHE STRING "mbedtls config file")
set(TF_PSA_CRYPTO_CONFIG_FILE ${CMAKE_SOURCE_DIR}/mbedtls-config/crypto_config.h
CACHE STRING "mbedtls crypto config file")

add_subdirectory(mbedtls)

#####################
# Create Executable #
#####################

file(GLOB_RECURSE IMPL_SRC CONFIGURE_DEPENDS
  src/impl/*.c
)

add_executable(hsm)
target_sources(hsm PRIVATE ${IMPL_SRC})

if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
  file(GLOB_RECURSE MSPN_SRC CONFIGURE_DEPENDS
    src/mspn/*.c
  )
  target_sources(hsm PRIVATE ${MSPN_SRC})
else()
  file(GLOB_RECURSE SIMU_SRC CONFIGURE_DEPENDS
    src/simu/*.c
  )
  target_sources(hsm PRIVATE ${SIMU_SRC})
endif()

target_link_libraries(hsm PRIVATE tfpsacrypto)

set_target_properties(hsm PROPERTIES SUFFIX .elf)


if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
  add_custom_command(TARGET hsm POST_BUILD
      COMMAND ${TI_LLVM}/bin/tiarmobjcopy -O binary hsm.elf hsm.bin
  )
endif()

###########
# Linting #
###########

if (NOT "${BUILD_MODE}" STREQUAL "SIMU")
  add_custom_target(lint ${TI_LLVM}/bin/tiarmclang-tidy ${SRC_FILES})
  add_custom_target(lint-ci ${TI_LLVM}/bin/tiarmclang-tidy --warnings-as-errors=* ${SRC_FILES})
endif()

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
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/docs
    )
endif()
