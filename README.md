## About

An extension version of [left4dhooks](https://forums.alliedmods.net/showthread.php?t=321696)

- Lite version. Most features are missing.
- As a C++ learning project. No compatibility guaranteed.

## Requirements:
- L4D2 dedicated server
- Sourcemod 1.11 or later

## Build manually
```bash
## Debian as an example.
apt update && apt install -y apt-transport-https lsb-release wget curl software-properties-common gnupg g++-multilib git make
bash <(curl -fsSL https://apt.llvm.org/llvm.sh) 18

echo 'export PATH=/usr/lib/llvm-18/bin:$PATH' >> /etc/profile
echo 'export CC=clang' >> /etc/profile
echo 'export CXX=clang++' >> /etc/profile
echo 'export XMAKE_ROOT=y' >> /etc/profile
source /etc/profile

wget https://xmake.io/shget.text -O - | bash
source ~/.xmake/profile

mkdir temp && cd temp
git clone --depth=1 -b 1.12-dev --recurse-submodules https://github.com/alliedmodders/sourcemod sourcemod
git clone --depth=1 -b l4d2 https://github.com/alliedmodders/hl2sdk hl2sdk-l4d2
git clone --depth=1 https://github.com/alliedmodders/safetyhook safetyhook
git clone --depth=1 https://github.com/fdxx/left4dhooks

cd left4dhooks
xmake f -c --SMPATH=../sourcemod --HL2SDKPATH=../hl2sdk-l4d2 --SAFETYHOOKPATH=../safetyhook
xmake -rv left4dhooks

## Check the left4dhooks/release folder.
```
