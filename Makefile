BUILDDIR=build
OPTIMIZE=-O0 -g #fast -msse4 -mfpmath=sse -march=native -mpopcnt -mbmi2  -fno-pie -fno-PIE -fno-PIC -no-pie 
ELFNAME=balash

CXX=g++
LIBS= -lpng16 -lglfw3 -lglew32 -lpng -DBUILD_SHARED_LIBS=ON -LC:\MinGW\lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32s

ifeq ($(OS),Windows_NT)
	LIBS = -lHELL0
else
    LIBS += `if [[ $OSTYPE == 'darwin'* ]]; then echo '-framework OpenGL'; fi`
endif


HEADERS=$(wildcard include/*.hpp) $(wildcard *.hpp)
CPPFLAGS= $(OPTIMIZE) -Wall $(LIBS)#remove -g when release

SOURCE:=$(wildcard *.cpp)
OBJS:=$(SOURCE:.cpp=.o) 

default:$(HEADERS) all run 

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
	sudo ln -sf /opt/homebrew/lib/libglib-2.0.0.dylib /usr/local/lib/libglib-2.0.0.dylib
all:$(BUILDDIR)/$(ELFNAME) Makefile 

getexec:#later for bloatstudio
	@echo $(BUILDDIR)/$(ELFNAME)

domestos:#really. added domestos XD
	rm -f $(OBJS) $(ASMOBJS)

mrproper:
	rm -rf $(BUILDDIR)
	mkdir $(BUILDDIR)

$(BUILDDIR)/$(ELFNAME): $(OBJS)
	$(CXX) $(OBJS) -o $(BUILDDIR)/$(ELFNAME) $(LIBS)