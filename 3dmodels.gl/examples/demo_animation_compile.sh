#!/bin/bash
set -eu

# Call this from ../../ (or just use `make examples').

fpc -dRELEASE @kambi.cfg 3dmodels.gl/examples/demo_animation.dpr