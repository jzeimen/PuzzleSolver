#! /bin/bash

touch NEWS README AUTHORS
autoreconf -vif && \
    ./configure && \
    make
