#!/usr/bin/env xonsh

import os

SCRIPT_SOURCE_DIR = os.path.realpath(os.path.dirname(__file__))
cd @(SCRIPT_SOURCE_DIR)
ARGS = "".join($ARGS).lower()

if not $(which idf.py):
    if os.path.exists(f"{os.path.expanduser('~')}/esp/esp-idf/export.sh"):
        print("Running esp-idf/export.sh")
        source-bash $HOME/esp/esp-idf/export.sh
        $PATH=$PATH
    else:
        print("idf.py not found")
        os.exit(1)
else:
    print("idf.py already on path")

mkdir -p "$SCRIPT_SOURCE_DIR/build"

if "b" in ARGS:

    cd @(SCRIPT_SOURCE_DIR)/build

    if "d" in ARGS:
        print("building in DEV_MODE")
        cmake .. -G Ninja -DDEV_MODE=1
    else:
        print("building")
        cmake .. -G Ninja

    ninja

    cd @(SCRIPT_SOURCE_DIR)

if "f" in ARGS:
    idf.py flash
