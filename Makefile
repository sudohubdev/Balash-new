BUILDDIR=build
OPTIMIZE=-O0 -g #fast -msse4 -mfpmath=sse -march=native -mpopcnt -mbmi2  -fno-pie -fno-PIE -fno-PIC -no-pie 
ELFNAME=balash

CXX=g++
LIBS = -O0

ifeq ($(OS),Windows_NT)
	LIBS = -lpng16 -lglfw3 -lglew32 -lpng -lopengl32
else
    LIBS = -lpng16 -lglfw -lglew -framework OpenGL
	OPERATING_SYSTEM := $(shell uname -s)
	ifeq ($(OPERATING_SYSTEM),Linux)
		LIBS = `pkg-config --libs --cflags glew libpng16 glfw3`
	endif
endif

LIBS=-fsanitize=address -static-libasan `pkg-config --libs --cflags glew libpng16 glfw3`

HEADERS=$(wildcard include/*.hpp) $(wildcard *.hpp) $(wildcard loaders/*.hpp) $(wildcard utils/*.hpp) 

SOURCE:=$(wildcard *.cpp) $(wildcard loaders/*.cpp) $(wildcard utils/*.cpp) 
OBJS:=$(SOURCE:.cpp=.o) 

default:$(HEADERS) mkdir all run 

mkdir:
	mkdir -p $(BUILDDIR)
run:
	./$(BUILDDIR)/$(ELFNAME)
macos:
	sudo ln -sf /opt/homebrew/Cellar/glew/*/include/GL /usr/local/include/GL
	sudo ln -sf /opt/homebrew/Cellar/glfw/*/include/GLFW /usr/local/include/GLFW
	#sudo ln -sf /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Headers /usr/local/include/GL
	sudo ln -sf /opt/homebrew/Cellar/glm/*/include/glm /usr/local/include/glm
	sudo ln -sf /opt/homebrew/Cellar/libpng/*/include/libpng16 /usr/local/include/libpng16
	
	sudo ln -sf /opt/homebrew/Cellar/glew/*/lib/libGLEW.dylib /usr/local/lib/libGLEW.dylib
	sudo ln -sf /opt/homebrew/Cellar/glfw/*/lib/libglfw.dylib /usr/local/lib/libglfw.dylib
	sudo ln -sf /opt/homebrew/Cellar/glm/*/lib/libglm.dylib /usr/local/lib/libglm.dylib
	sudo ln -sf /opt/homebrew/Cellar/libpng/*/lib/libpng16.dylib /usr/local/lib/libpng16.dylib
	sudo ln -sf /opt/homebrew/Cellar/freeglut/*/lib/libglut.dylib /usr/local/lib/libglut.dylib
	sudo ln -sf /opt/homebrew/lib/libglib-2.0.0.dylib /usr/local/lib/libglib-2.0.0.dylib
all:$(BUILDDIR)/$(ELFNAME) Makefile 

getexec:#later for bloatstudio
	@echo $(BUILDDIR)/$(ELFNAME)

domestos:#really. added domestos XD
	rm -f $(OBJS) $(ASMOBJS)

newproject:
	rm -f *.obj 
	rm -f *.png 
	rm -f *.cpp 
	rm -f *.hpp 
	rm -f *.o 
	rm -f *.vsh 
	rm -f *.fsh

mrproper:
	rm -rf $(BUILDDIR)
	mkdir $(BUILDDIR)

$(BUILDDIR)/$(ELFNAME): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) $(OPTIMIZE) -o $(BUILDDIR)/$(ELFNAME)