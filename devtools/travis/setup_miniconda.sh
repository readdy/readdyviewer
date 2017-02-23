#!/usr/bin/env bash
if [ "$TRAVIS_OS_NAME" = "linux" ]; then OS=Linux-x86_64; else OS=MacOSX-x86_64; fi
wget -O miniconda.sh https://repo.continuum.io/miniconda/Miniconda${CONDA_PY:0:1}-latest-$OS.sh
bash miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
conda config --set always_yes yes --set changeps1 no
conda update -q conda
conda install -q conda-build
echo "creating conda env with python=${CONDA_PY}"
conda create -q -n test-environment python=${CONDA_PY}
echo "available conda envs:"
conda info --envs
source activate test-environment