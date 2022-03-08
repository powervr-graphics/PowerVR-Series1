#
# Makefile for example PROG
#
CC = gcc
INCLUDES = -I../..

PROG = trantower
SRC  = trantower.c
SGLLIB = ../../sgl.a

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
	mv result00.bmp expected.bmp


#
# End of makefile
#
