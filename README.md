# Balash-new - rewrite of balash
the example of opengl engine
# Build
## For mac users:
### Install brew + xcode
### Then run commands in terminal:
```bash
git clone https://github.com/g0vnocoders/balash-new
brew install glew
brew install glm
make macos
make
```
## For Windows users:
### Download MinGW and VS Code

https://www.mingw-w64.org
https://code.visualstudio.com

### Then run commands in terminal(MSYS2 MinGW64 blue icon in start menu):
```bash
# open it with C:\msys64\mingw64.exe!!!  not cmd.exe
git clone https://github.com/g0vnocoders/balash-new
cd balash-new
makepkg -sCLF
make
```


## For Linux users:
### You already know what to do, dont ya?
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install cmake pkg-config
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev
sudo apt-get install libglew-dev libglfw3-dev libglm-dev
sudo apt-get install libao-dev libmpg123-dev
sudo apt-get install libpng-dev
git clone https://github.com/g0vnocoders/balash-new
cd balash-new
make
```