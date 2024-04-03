# set this variable to the director in which you saved the common files
commondir = ../common/
libdir = ../lib/

dependencies = $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)/MicroGlut.m $(libdir)/boxes.cpp $(libdir)/boxes.h
flags = -Wall -DGL_GLEXT_PROTOTYPES -lm -lstdc++ -framework OpenGL -framework Cocoa -framework CoreGraphics

main : main.cpp $(dependencies)
	gcc -o main \
	-I $(commondir) \
	-I $(libdir) \
	main.cpp \
	$(commondir)GL_utilities.c \
	$(commondir)LoadTGA.c \
	$(commondir)/MicroGlut.m \
	$(libdir)/boxes.cpp \
	$(flags)

clean :
	rm main
