# set this variable to the director in which you saved the common files
COMMONDIR = ../common/
LIBDIR = lib/


CC = gcc
CXX = g++
CFLAGS = -Wall -DGL_GLEXT_PROTOTYPES
CXXFLAGS = $(CFLAGS) -std=c++17
LDFLAGS = -lm -framework OpenGL -framework Cocoa -framework CoreGraphics

C_SOURCES = \
$(COMMONDIR)GL_utilities.c \
$(COMMONDIR)LoadTGA.c \
$(COMMONDIR)/MicroGlut.m \

CPP_SOURCES = \
$(LIBDIR)/boxes.cpp \
$(LIBDIR)/ground.cpp \
$(LIBDIR)/systems/RenderSystem.cpp \
$(LIBDIR)/systems/CameraControlSystem.cpp \
$(LIBDIR)/mapGen.cpp \

# Object files
C_OBJECTS := $(C_SOURCES:.c=.o)
CPP_OBJECTS := $(CPP_SOURCES:.cpp=.o)

main: main.o $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) -o main main.o $(C_OBJECTS) $(CPP_OBJECTS) $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -I $(COMMONDIR) -I $(LIBDIR) -c main.cpp -o main.o

# Pattern rule for C source files
%.o: %.c
	$(CC) $(CFLAGS) -I $(COMMONDIR) -I $(LIBDIR) -c $< -o $@

# Pattern rule for C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I $(COMMONDIR) -I $(LIBDIR) -c $< -o $@

clean :
	rm main
