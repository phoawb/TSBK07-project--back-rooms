# set this variable to the director in which you saved the common files
commondir = ../common/

main : main.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o main -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES main.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

clean :
	rm main
