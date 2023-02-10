# Balash-new - rewrite of balash
the example of opengl engine

<img src="https://user-images.githubusercontent.com/36532941/218088488-415a3216-e5b1-4130-ac55-2d6f88932f43.png"  width="400px"/>

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
### Download MSYS2 and VS Code(or any other IDE, tho `VS will not work!`)

https://www.msys2.org

https://code.visualstudio.com

### Then run commands in terminal(__MSYS2 MinGW64 blue icon in start menu__):
```bash
# open it with C:\msys64\mingw64.exe!!!  not cmd.exe
pacman -S git
pacman -S mingw-w64-x86_64-gcc
git clone https://github.com/g0vnocoders/balash-new
cd balash-new
makepkg -sCL
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


# To make a new project(delete all cpp files):
```bash
make newproject
```
