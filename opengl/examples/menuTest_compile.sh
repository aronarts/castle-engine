#!/bin/bash
set -eu

# Call this from ../../ (or just use `make examples').

fpc -dRELEASE @kambi.cfg opengl/examples/menuTest.dpr