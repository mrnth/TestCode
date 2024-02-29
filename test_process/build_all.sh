#!/bin/bash

cd child_process_0
make -j
cd ../child_process_1
make -j
cd ../parent_process
make -j
