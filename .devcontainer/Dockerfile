FROM quay.io/fedora/fedora

COPY .devcontainer/init.bash .

RUN bash init.bash

RUN mkdir /tmp/scripts
COPY mbedtls/scripts/*.txt /tmp/scripts/
RUN pip install -r /tmp/scripts/basic.requirements.txt








