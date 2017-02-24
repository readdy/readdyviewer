#!/usr/bin/env bash
echo "got truncated conda version:"
echo "--------"
echo ${PY_VER}
echo "--------"
if [ "$TRAVIS_OS_NAME" = "linux" ]; then OS=Linux-x86_64; else OS=MacOSX-x86_64; fi
wget -O miniconda.sh https://repo.continuum.io/miniconda/Miniconda3-latest-$OS.sh
bash miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
conda config --set always_yes yes --set changeps1 no
conda update -q conda
conda install -q conda-build
echo "creating conda env with python ${python}"
conda create -q -n test-environment python=${python}
echo "available conda envs:"
conda info --envs