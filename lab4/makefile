# set this variable to the director in which you saved the common files
commondir = ../common/

all : lab4-1 lab4-2 lab4-3 lab4-4 lab4-5 lab4-6

lab4-1 : lab4-1.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-1 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-1.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

lab4-2 : lab4-2.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-2 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-2.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

lab4-3 : lab4-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-3 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

lab4-4 : lab4-4.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-4 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-4.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

lab4-5 : lab4-5.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-5 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-5.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

lab4-6 : lab4-6.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m
	gcc -Wall -o lab4-6 -I$(commondir) -I../common/ -DGL_GLEXT_PROTOTYPES lab4-6.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m  -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

clean :
	rm lab4-1 lab4-2 lab4-3 lab4-4 lab4-5 lab4-6
