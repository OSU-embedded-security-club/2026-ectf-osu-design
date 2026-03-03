#!/bin/bash

source /etc/bashrc
echo "Sourced bashrc"

cd /hsm
echo "Changed to /hsm"

python3 generate_secrets.py /secrets/global.secrets ${HSM_PIN} "${PERMISSIONS}"
echo "Generated secrets"

cd /out
echo "Changed to /out"

cmake -S /hsm -DCMAKE_BUILD_TYPE=Release .
make
echo "Built HSM"