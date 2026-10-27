#!/bin/sh
#SBATCH --job-name=build
#SBATCH --output=build-out/slurm-%j.out
#SBATCH --error=build-out/slurm-%j.err

set -e
cmake --preset clang-tidy
cmake --build --preset clang-tidy-release
echo "Finished building: clang-tidy-release"
