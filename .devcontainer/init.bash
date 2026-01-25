#!/bin/bash

sudo dnf install -y wget curl git cmake clangd uv doxygen python3-pip

curl --proto '=https' --tlsv1.2 -LsSf https://github.com/probe-rs/probe-rs/releases/latest/download/probe-rs-tools-installer.sh | sh

wget -O ti_llvm_installer.bin https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-ayxs93eZNN/4.0.2.LTS/ti_cgt_armllvm_4.0.2.LTS_linux-x64_installer.bin
chmod +x ti_llvm_installer.bin

mkdir /ti
./ti_llvm_installer.bin --mode unattended --prefix /ti

echo 'export TI_LLVM_ROOT=/ti/ti-cgt-armllvm_4.0.2.LTS' >> /etc/bashrc
echo 'export PATH=$PATH:/ti/ti-cgt-armllvm_4.0.2.LTS/bin' >> /etc/bashrc
