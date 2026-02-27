#!/bin/bash

source /etc/bashrc

cd /hsm

python3 generate_secrets.py /secrets/global.secrets ${HSM_PIN} "${PERMISSIONS}"

cd /out

cmake -S /hsm -DCMAKE_BUILD_TYPE=Release .
make