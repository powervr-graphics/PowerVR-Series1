#
# Makefile for example PROG
#
CC = gcc
INCLUDES = -I../..
SGLLIB = ../../sgl.a

PROG = tube
SRC  = tube.c

#
# Build the program
#
$(PROG): $(SRC) $(SGLLIB)
	$(CC) -o $(PROG) $(INCLUDES) $(SRC) $(SGLLIB)  -lm

#
# release
#
release:    expected.bmp


expected.bmp: $(PROG)
	$(PROG)
	mv result.bmp expected.bmp


#
# End of makefile
#
