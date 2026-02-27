#!/bin/bash

source /etc/bashrc

cd /hsm

git clone https://github.com/LoupVaillant/Monocypher.git
git clone https://github.com/TexasInstruments/mspm0-sdk.git

python3 generate_secrets.py /secrets/global.secrets ${HSM_PIN} "${PERMISSIONS}"

cd /out

cmake /hsm
make