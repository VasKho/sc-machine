[![codecov](https://codecov.io/gh/ostis-ai/sc-machine/branch/main/graph/badge.svg?token=WU8O9Z1DNL)](https://codecov.io/gh/ostis-ai/sc-machine)

## Clone:

```sh
git clone https://github.com/ostis-ai/sc-machine.git
cd sc-machine
git submodule update --init --recursive
```

## Install dependencies

### Debian-based (Ubuntu, Debian, Mint)

```sh
cd scripts
./install_deps_ubuntu.sh
cd ..
pip3 install wheel setuptools
pip3 install -r requirements.txt
```

### macOS
```sh
cd scripts
./install_deps_macOS.sh
cd ..
pip3 install wheel setuptools
pip3 install -r requirements.txt
```
Please note: you should add Qt5 and LLVM to `PATH` variable. To do this, after installing dependencies execute the following commands (considering you use `zsh` as your shell):
```sh
echo 'export PATH="'$HOMEBREW_PREFIX'/opt/qt@5/bin:$PATH"' >> ~/.zshrc
echo 'export PATH="'$HOMEBREW_PREFIX'/opt/llvm/bin:$PATH"' >> ~/.zshrc
```

## Build sc-machine
```sh
cd sc-machine
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # use Debug for debug build
make
```

## Build knowledge base:

This repo provides *build_kb.py* script to build and prepare knowledge base (KB).

### Usage
build_kb.py [-h] [-o OUTPUT_PATH] [-l ERRORS_FILE_PATH] [-f REPO_PATH_NAME]
repo_folder

positional arguments:

repo_folder The entrypoint folder, should contain a repo file (repo.path by default)

options:

  -h, -\-help -- show this help message and exit

  -o OUTPUT_PATH, -\-output OUTPUT_PATH -- Destination path - path where prepared KB and built KB (kb.bin) will be stored. Default: \<cwd>

  -l ERRORS_FILE_PATH, -\-logfile ERRORS_FILE_PATH -- Errors file path - in case of unsuccessful preparation, log will appear at this location. Default: \<cwd>/prepare.log

  -f REPO_PATH_NAME, -\-filename REPO_PATH_NAME -- Repo file name - a filename for repo file that will be used in all subsequent KBs. Default: repo.path

*Example: (\<cwd> is ./sc-machine)*
```sh
# This command will parse repo.path in current directory
# and create kb.bin in ../ directory. If errors occured
# ./prepare.log will be created

python scripts/build_kb.py ./ -o ../ -f repo.path -l ./
```

### repo.path example
```sh
# Comments are allowed like this
# Here you can specify path to kb
../ims.ostis.kb
```

## Servers

There are two possible ways to use *sc-machine*:

1. **Sc-server**

  Use *./sctipts/run_sc_sever.sh* script to run sc-server
  ```
  ./run_sc_server.sh
  ```
2. **Sctp server**

  Use *./sctipts/run_sctp.sh* script to run sctp server
  ```
  ./run_sctp.sh
  ```

*This repository continues the development of [this sc-machine](https://github.com/ostis-dev/sc-machine) from version 0.6.0.*