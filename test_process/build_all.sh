#!/bin/bash

cd child_process_0
make clean
make -j
cd ../child_process_1
make clean
make -j
cd ../parent_process
make clean
make -j
