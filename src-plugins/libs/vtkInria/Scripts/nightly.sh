#!/bin/bash

export PKG_CONFIG_PATH=/proj/asclepios/home/import/gtkglext/lib/pkgconfig:$PKG_CONFIG_PATH

export SCRIPT_DIR=/home/dashboardBuilds/vtkINRIA3D/src/Scripts
cd $SCRIPT_DIR
/proj/asclepios/home/import/CMake/testing/install/linux_FC3/bin/ctest -S nightly.cmake
