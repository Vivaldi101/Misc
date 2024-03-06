#! /bin/bash

./configure --with-features=huge \
            --enable-multibyte \
            --enable-gui=gtk3 \
            --enable-python3interp=yes \
            --with-python3-command=python3.8 \
            --with-python3-config-dir=$(python3.8-config --configdir) \
            --enable-gtk3-check \
            --prefix=/usr/local \
