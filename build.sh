#!/bin/sh
rm -f config.h
make -j$(nproc)
