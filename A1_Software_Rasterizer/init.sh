#!/bin/bash

# Deactivate any active conda environment
conda deactivate

# Activate the 'graphics' conda environment
conda activate graphics

# Source the bashrc file (if necessary)
source ~/.bashrc

# Remove the existing build directory
rm -rf build

# Run CMake to generate build files
cmake -B build

# Notify completion
echo "Build setup complete."