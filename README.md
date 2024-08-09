## About

An extension version of [left4dhooks](https://forums.alliedmods.net/showthread.php?t=321696)

- Lite version. Most features are missing.
- As a C++ learning project. No compatibility guaranteed.

## Requirements:
- L4D2 dedicated server
- Sourcemod 1.11 or later

## Build manually
```sh
## Debian as an example.
dpkg --add-architecture i386
apt update && apt install -y clang g++-multilib wget git make

export CC=clang && export CXX=clang++
wget https://xmake.io/shget.text -O - | bash

mkdir temp && cd temp
git clone --depth=1 -b 1.11-dev --recurse-submodules https://github.com/alliedmodders/sourcemod sourcemod
git clone --depth=1 -b l4d2 https://github.com/alliedmodders/hl2sdk hl2sdk-l4d2
git clone --depth=1 https://github.com/alliedmodders/safetyhook safetyhook
git clone --depth=1 https://github.com/fdxx/left4dhooks

cd left4dhooks
xmake f --SMPATH=../sourcemod --HL2SDKPATH=../hl2sdk-l4d2 --SAFETYHOOKPATH=../safetyhook
xmake -rv left4dhooks

## Check the left4dhooks/release folder.
```
