pacman -S make
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-gcc
pacman -S git
pacman -S wget

pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-glm


# build libpng16
wget https://sourceforge.net/projects/libpng/files/libpng16/1.6.39/libpng-1.6.39.tar.gz/download
tar -xvf download
cd libpng-1.6.39
./configure --prefix=$PREFIX --enable-shared --enable-static
make
make install
cd ..
make 